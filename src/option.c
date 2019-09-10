#include <stdio.h>
#include <string.h>
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
  else if (strcmp(WJEString(protojson,"schema.type",WJE_GET,"unknown"),"object") == 0)
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
    if (WJEGetF(optionlist(protojson), NULL, "schema.items.properties.%s", optionname))
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
    if (WJEGetF(optionlist(rl_protojson), NULL, "schema.items.properties.%s", optionname))
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
    if (argv[1][0] == '\"')
    {
      argv[1] = cutquot(argv[1]);
    }
    WJEString(protoface, parameter->name, WJE_SET, argv[1]);
  }
  puts(WJEString(protoface,parameter->name,WJE_GET,"<undefined>"));
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
  if (!state)
  {
   if (rl_parameter) 
   {
     return strdup(WJEString(rl_protoface, rl_parameter->name, WJE_GET, ""));
   }
  }
  return NULL;
}

