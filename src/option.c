#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <wjelement.h>
#include <sys/stat.h>
#include "zcore.h"
#include "interpreter.h"
#include "option.h"
#include "utils.h"
#include "completion.h"
#include "builtin.h"
#include "config.h"
#include "validate.h"
#include "prompt.h"
#include "setting.h"

extern WJElement protojson;
extern WJElement protoschema;
extern WJElement protoface;
extern WJElement rl_protojson;
extern WJElement rl_protoschema;
extern WJElement rl_protoface;
extern char facename[100];
extern enum domains domain;
WJElement parameter = NULL;
WJElement rl_parameter = NULL;
WJElement optionjson = NULL;
int optiondepth = 0;

int listoptions(void)
{
  WJElement option = NULL;
  puts("Options:");

  while ((option = _WJEObject(optionlist(protoschema, protoface->name), "properties[]", WJE_GET, &option))) {
    if (!WJEBool(option, "hidden", WJE_GET, FALSE))
    {
      printf("%s: ", option->name);
      char * optionstring = optionhelp(option->name, protoschema, protoface);
      if (optionstring)
      {
        printf("%s\n", optionstring);
        free(optionstring);
      }
      else
      {
        printf("None\n");
      }
    }
  }

  return 0;
}

int isoption(char * optionname)
{
  if (domain == OPTION)
  {
    if (WJEGetF(optionlist(protoschema, protoface->name), NULL, "properties.%s", optionname))
    {
      return 1;
    }
  }
  return 0;
}

int rl_isoption(char * optionname)
{
  if (rl_domain == OPTION)
  {
    if (WJEGetF(optionlist(rl_protoschema, rl_protoface->name), NULL, "properties.%s", optionname))
    {
      return 1;
    }
  }
  return 0;
}

int option(int argc, char *argv[])
{
  parameter = conditionoption(protoschema, protoface, argv[0]);

  if (WJEGet(parameter, "[\"$ref\"]", NULL))
  {
    parameter = WJEGetF(root, NULL, "%s.schema", WJEString(parameter, "[\"$ref\"]", WJE_GET, NULL));
  }

  if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"object") == 0)
  {
    if (WJEGet(protoface,argv[0],NULL))
    {
      protoschema = WJEGetF(optionlist(protoschema, protoface->name),NULL,"properties.%s",argv[0]);// ha ha
      protoface = WJEGet(protoface,argv[0],NULL);
      optiondepth++;
      if (argc > 1)
       if (isoption(argv[1]))
         return option(argc-1, &argv[1]);
      return 0;
    }
    else
    {
      puts("cannot traverse empty object");
      return 1;
    }
  }
  else
  {
    //domain = SETTING;
    //return setting(argc-1, &argv[1]);

    if (argc == 1)
    {
      char * returnstring = optionvalue(argv[0], protoschema, protoface);
      if (returnstring)
      {
        puts(returnstring);
        free(returnstring);
      }
      else
        puts("undefined");
      return 1;
    }
    else if (argc == 2)
    {
      if (WJEBool(parameter,"readonly",WJE_GET,FALSE))
      {
        puts("option readonly");
        return 1;
      }
      return option_set_value(parameter, argv[0], argv[1]);
    }
    else if (argc > 2)
    {
      if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0)
      {
        for (int i = 1; i < argc; i++)
        {
          option_set_value(parameter, argv[0], argv[i]);
        }
      }
      else
      {
        char combine[1000] = "";
        strcpy(combine, argv[1]);
        for (int i = 2; i < argc; i++)
        {
          strcat(combine, " ");
          strcat(combine, argv[i]);
        }
        return option_set_value(parameter, argv[0], combine);
      }
    }
  }

  return 1;
}

int rl_option(int argc, char *argv[])
{
  WJECloseDocument(rl_parameter);
  rl_parameter = WJEObject(NULL, argv[0], WJE_NEW);


  char * facename = NULL;
  if (rl_protoface)
    facename = rl_protoface->name;
  else
    facename = NULL;

  WJElement anyoption = NULL;
  while (anyoption = _WJEObject(optionlist(rl_protoschema, facename), "anyOf[]", WJE_GET, &anyoption)) {
    if (WJESchemaValidate(anyoption, rl_protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      WJEMergeObjects(rl_parameter, WJEObjectF(anyoption, WJE_GET, NULL, "properties.%s",argv[0]), TRUE);
    }
  }

  WJEMergeObjects(rl_parameter, WJEObjectF(optionlist(rl_protoschema, facename), WJE_GET, NULL, "properties.%s",argv[0]), TRUE);

  if (strcmp(WJEString(rl_parameter,"type", WJE_GET, NULL),"object") == 0)
  {
    rl_protoface = WJEGet(rl_protoface,argv[0],NULL);
    rl_protoschema = WJEGetF(rl_protoschema,NULL,"properties.%s",argv[0]);
    if (argc > 1)
      if (rl_isoption(argv[1]))
        return rl_option(argc-1, &argv[1]);
    else
      return 1;
  }
  else
  {
    rl_domain = SETTING;
    return 1;
  }
}

char * optionvalues(const char * text, int len)
{
  char * suggest_name = NULL;
  if (rl_protoface)
    suggest_name = rl_protoface->name;

  static WJElement option = NULL;
  while (option = _WJEObject(optionlist(rl_protoschema, suggest_name), "properties[]", WJE_GET, &option)) {
    if (WJEBool(option, "hidden", WJE_GET, FALSE))
      return optionvalues(text,len);
    if (strncmp(option->name, text, len) == 0) {
      return strdup(option->name);
    }
  }
  return NULL;
}

char * optionhelp(const char * commandname, WJElement proto, WJElement face)
{
  WJElement parameter;
  char * returnstring = NULL;
  parameter = WJEObjectF(optionlist(proto, face->name), WJE_GET, NULL, "properties.%s",commandname);
  if (WJEGet(parameter, "[\"$ref\"]", NULL))
    parameter = WJEGetF(root, NULL, "%s.schema", WJEString(parameter, "[\"$ref\"]", WJE_GET, NULL));

  if (WJEGet(parameter, "description", NULL))
    return strdup(WJEString(parameter,"description", WJE_GET, NULL));
  else
    return NULL;
}
