#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <wjelement.h>
#include <sys/stat.h>
#include <regex.h>
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
char * optionname = NULL;
WJElement optionlistone = NULL;
WJElement rl_optionlistone = NULL;

WJElement optionlist(WJElement schema, char * protoname)
{
  WJElement properties = NULL;
  if (WJEGet(schema, "patternProperties", NULL))
  {
    if (protoname == NULL)
      return WJEObject(schema,"patternProperties[0]", WJE_GET);

    regex_t preg;
    while (properties = _WJEObject(schema, "patternProperties[]", WJE_GET, &properties))
    {
      regcomp(&preg, properties->name, REG_EXTENDED | REG_NOSUB);
      if (!regexec(&preg, protoname, 0, NULL, 0))
      {
        return properties;
      }
    }
  }
  else
    return schema;
}

WJElement anyoption(void)
{
  WJElement optionlisttwo = WJEObject(NULL, NULL, WJE_NEW);
  WJECopyDocument(optionlisttwo, optionlist(protoschema, protoface->name), NULL, NULL);

  WJElement anyoption = NULL;
  while (anyoption = _WJEObject(optionlist(protoschema, protoface->name), "anyOf[]", WJE_GET, &anyoption)) {
    WJElement tempschema = WJEObject(NULL, NULL, WJE_NEW);
    WJECopyDocument(tempschema, anyoption, NULL, NULL);
    WJElement tempdefs = WJEObject(NULL, "definitions", WJE_NEW);
    if (WJEGet(protoschema, "definitions", NULL))
      WJECopyDocument(tempdefs, WJEGet(protoschema, "definitions", NULL), NULL, NULL);
    WJEAttach(tempschema, tempdefs);
    if (WJESchemaValidate(tempschema, protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      WJEMergeObjects(optionlisttwo, anyoption, TRUE);
    }
    WJECloseDocument(tempschema);
  }

  return optionlisttwo;
}

WJElement rl_anyoption(void)
{
  WJElement optionlistone = WJEObject(NULL, NULL, WJE_NEW);
  WJECopyDocument(optionlistone, optionlist(rl_protoschema, rl_protoface->name), NULL, NULL);

  WJElement anyoption = NULL;
  while (anyoption = _WJEObject(optionlist(rl_protoschema, rl_protoface->name), "anyOf[]", WJE_GET, &anyoption)) {
    WJElement tempschema = WJEObject(NULL, NULL, WJE_NEW);
    WJECopyDocument(tempschema, anyoption, NULL, NULL);
    WJElement tempdefs = WJEObject(NULL, "definitions", WJE_NEW);
    if (WJEGet(rl_protoschema, "definitions", NULL))
      WJECopyDocument(tempdefs, WJEGet(rl_protoschema, "definitions", NULL), NULL, NULL);
    WJEAttach(tempschema, tempdefs);
    if (WJESchemaValidate(tempschema, rl_protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      WJEMergeObjects(optionlistone, anyoption, TRUE);
    }
    WJECloseDocument(tempschema);
  }

  return optionlistone;
}

int listoptions(void)
{
  if (optionlistone)
  {
    WJECloseDocument(optionlistone);
    optionlistone = NULL;
  }

  optionlistone = anyoption();

  puts("Options:");
  WJElement option = NULL;

  while ((option = _WJEObject(optionlistone, "properties[]", WJE_GET, &option))) {
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
    if (optionlistone)
    {
      WJECloseDocument(optionlistone);
      optionlistone = NULL;
    }
    optionlistone = anyoption();

    if (WJEGetF(optionlistone, NULL, "properties.%s", optionname))
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

WJElement conditionoption(WJElement schema, WJElement face, char * optionname)
{
  char * facename = NULL;
  if (face)
    facename = face->name;
  else
    facename = NULL;
  WJElement option_parameter = NULL;

  if (optionlistone)
  {
    WJECloseDocument(optionlistone);
    optionlistone = NULL;
  }
  optionlistone = anyoption();

  option_parameter = WJEObjectF(optionlistone, WJE_GET, NULL, "properties.%s",optionname);

  return option_parameter;
}

int option(int argc, char *argv[])
{
  optionname = argv[0];
  parameter = conditionoption(protoschema, protoface, optionname);

  if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"object") == 0)
  {
    if (WJEGet(protoface,optionname,NULL))
    {
      protoschema = WJEGetF(optionlist(protoschema, protoface->name),NULL,"properties.%s",argv[0]);// ha ha
      protoface = WJEGet(protoface,optionname,NULL);
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
    domain = SETTING;
    return setting(argc-1, &argv[1]);
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

    //taks taks taks
    WJElement tempschema = WJEObject(NULL, NULL, WJE_NEW);
    WJECopyDocument(tempschema, anyoption, NULL, NULL);

    WJElement tempdefs = WJEObject(NULL, "definitions", WJE_NEW);

    if (WJEGet(rl_protoschema, "definitions", NULL))
      WJECopyDocument(tempdefs, WJEGet(rl_protoschema, "definitions", NULL), NULL, NULL);

    WJEAttach(tempschema, tempdefs);

    //puts("check");
    //WJEDump(tempschema);

    if (WJESchemaValidate(tempschema, rl_protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      //puts("compile");
      //WJEDump(tempschema);
      //WJEMergeObjects(rl_parameter, WJEObjectF(anyoption, WJE_GET, NULL, "properties.%s",argv[0]), TRUE);
      WJEMergeObjects(rl_parameter, conditionoption(tempschema, rl_protoface, argv[0]), TRUE);
    }

    WJECloseDocument(tempschema);
  }

  //puts("12one");
  //WJEDump(rl_parameter);

  WJEMergeObjects(rl_parameter, conditionoption(rl_protoschema, rl_protoface, argv[0]), TRUE);

  //puts("12two");
  //WJEDump(rl_parameter);

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

  if (option == NULL)
  {
    if (rl_optionlistone)
    {
      WJECloseDocument(rl_optionlistone);
      rl_optionlistone = NULL;
    }
    rl_optionlistone = rl_anyoption();
  }

  while (option = _WJEObject(rl_optionlistone, "properties[]", WJE_GET, &option)) {
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
