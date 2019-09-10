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
WJElement rl_parameter;

int listoptions(void)
{
  WJElement option = NULL;
  puts("Options:");

  while ((option = _WJEObject(protojson, "schema.items.properties[]", WJE_GET, &option))) {
    puts(option->name);
  }
}

int isoption(char * optionname)
{
  if (domain == OPTION)
  {
    if (WJEGetF(protojson, NULL, "schema.items.properties.%s", optionname))
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
    if (WJEGetF(rl_protojson, NULL, "schema.items.properties.%s", optionname))
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
//  WJElement parameter;
//  parameter = WJEObjectF(protojson, WJE_GET, NULL, "schema.items.properties.%s",argv[0]);
//  if (argc == 2)
//  {
//    if (argv[1][0] == '\"')
//    {
//      argv[1] = cutquot(argv[1]);
//    }
//    WJEString(protoface, parameter->name, WJE_SET, argv[1]);
//  }
//  puts(WJEString(protoface,parameter->name,WJE_GET,"<undefined>"));
  puts(WJEString(protoface,argv[0],WJE_GET,"<undefined>"));
  return 1;
}

int rl_option(int argc, char *argv[])
{
  rl_parameter = WJEObjectF(rl_protojson, WJE_GET, NULL, "schema.items.properties.%s",argv[0]);
  rl_domain = SETTING;
  return 1;
}

char * optionvalues(const char * text, int len)
{
  static WJElement option = NULL;
  while (option = _WJEObject(rl_protojson, "schema.items.properties[]", WJE_GET, &option)) {
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

