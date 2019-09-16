#include <stdio.h>
#include <string.h>
#include <wjelement.h>
#include "zcore.h"
#include "interpreter.h"
#include "option.h"
#include "utils.h"
#include "completion.h"

extern WJElement protojson;
extern WJElement protoface;
extern WJElement rl_protojson;
extern WJElement rl_protoface;
extern WJElement rl_optionjson;
WJElement rl_parameter;

WJElement optionlist(WJElement schema)
{
  if (strcmp(WJEString(schema,"schema.type",WJE_GET,"unknown"),"array") == 0)
  {
    return WJEObject(schema,"schema.items", WJE_GET);
  }
  else if (strcmp(WJEString(schema,"schema.type",WJE_GET,"unknown"),"object") == 0)
  {
    return WJEObject(schema,"schema", WJE_GET);
  }
}

int listoptions(void)
{
  WJElement option = NULL;
  puts("Options:");

  while ((option = _WJEObject(optionlist(protojson), "properties[]", WJE_GET, &option))) {
    puts(option->name);
  }
}

int isoption(char * optionname)
{
  if (domain == OPTION)
  {
    if (WJEGetF(optionlist(protojson), NULL, "properties.%s", optionname))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

int rl_isoption(char * optionname)
{
  if (rl_domain == OPTION)
  {
    if (WJEGetF(optionlist(rl_protojson), NULL, "properties.%s", optionname))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

int option(int argc, char *argv[])
{
  WJElement parameter;
  parameter = WJEObjectF(optionlist(protojson), WJE_GET, NULL, "properties.%s",argv[0]);
  if (argc == 2)
  {
    if (argv[1][0] == '?')
    {
      puts(WJEString(parameter,"description",WJE_GET,"Help not found"));
      return 1;
    }
    if (argv[1][0] == '\"')
    {
      argv[1] = cutquot(argv[1]);
    }
    WJEString(protoface, parameter->name, WJE_SET, argv[1]);
  }
  if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"string") == 0)
  {
    puts(WJEString(protoface,parameter->name,WJE_GET,"<undefined>"));
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"number") == 0)
  {
    printf("%d\n", WJEInt32(protoface,parameter->name,WJE_GET,-1));
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"boolean") == 0)
  {

  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0)
  {
    WJElement array = NULL;
    char * entity = NULL;
    while (entity = WJEStringF(protoface, WJE_GET, &array, NULL, "%s[]", parameter->name))
    {
      puts(entity);
    }
  }
  else
  {
    puts("Not implemeted");
  }
  return 1;
}

int rl_option(int argc, char *argv[])
{
  rl_parameter = WJEObjectF(optionlist(rl_protojson), WJE_GET, NULL, "properties.%s",argv[0]);
  rl_domain = SETTING;
  return 1;
}

char * optionvalues(const char * text, int len)
{
  static WJElement option = NULL;
  while (option = _WJEObject(optionlist(rl_protojson), "properties[]", WJE_GET, &option)) {
    if (strncmp(option->name, text, len) == 0) {
      return strdup(option->name);
    }
  }
  return NULL;
}

char * settingvalues(const char * text, int len, int state)
{
  static WJElement setting = NULL;
  char * variant = NULL;
  while (variant = _WJEString(rl_parameter, "enum[]", WJE_GET, &setting, NULL))
  {
    if (strncmp(variant, text, len) == 0) {
      return strdup(variant);
    }
  }
  /*if (!state)
  {
   if (rl_parameter) 
   {
     return strdup(WJEString(rl_protoface, rl_parameter->name, WJE_GET, ""));
   }
  }*/
  return NULL;
}

