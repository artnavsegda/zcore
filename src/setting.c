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
extern WJElement parameter;
extern WJElement rl_parameter;
extern WJElement optionjson;
extern int optiondepth;
extern char * optionname;

char * optionvalue(char * commandname, WJElement proto, WJElement face)
{
  WJElement parameter;
  char * returnstring = NULL;

  parameter = conditionoption(proto, face, commandname);

  if (WJEGet(parameter, "[\"$ref\"]", NULL))
  {
    parameter = WJEGetF(root, NULL, "%s.schema", WJEString(parameter, "[\"$ref\"]", WJE_GET, NULL));
  }

  if (WJEGet(face, commandname, NULL))
  {
    if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"string") == 0){
      return strdup(WJEString(face,commandname,WJE_GET,"<undefined>"));
    }
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"number") == 0){
      asprintf(&returnstring,"%d", WJEInt32(face,commandname,WJE_GET,-1));
      return returnstring;
    }
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"boolean") == 0){
      if (WJEBool(face,commandname,WJE_GET,-1) == TRUE)
        return strdup("true");
      else if (WJEBool(face,commandname,WJE_GET,-1) == FALSE)
        return strdup("false");
    }
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0){
      WJElement array = NULL;
      if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"string") == 0){
        char * entity = NULL;
        returnstring = malloc(1);
        if (!returnstring)
        {
            puts("memory allocation 1 fail");
            return NULL;
        }
        returnstring[0] = '\0';
        while (entity = WJEStringF(face, WJE_GET, &array, NULL, "%s[]", commandname))
        {
          if (returnstring = realloc(returnstring, strlen(returnstring) + strlen(entity) + 2))
          {
            strcat(returnstring, entity);
            strcat(returnstring, " ");
          }
          else
          {
            puts("memory allocation 2 fail");
            return NULL;
          }
        }
        return returnstring;
      }
      else if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"number") == 0){
        int number = 0;
        returnstring = malloc(1);
        if (!returnstring)
        {
            puts("memory allocation 1 fail");
            return NULL;
        }
        returnstring[0] = '\0';
        while (number = WJEInt32F(face, WJE_GET, &array, 0, "%s[]", commandname))
        {
          char tmpstr[15];
          sprintf(tmpstr, "%d ", number);
          if (returnstring = realloc(returnstring, strlen(returnstring) + strlen(tmpstr) + 1))
          {
            strcat(returnstring, tmpstr);
          }
          else
          {
            puts("memory allocation 2 fail");
            return NULL;
          }
        }
        return returnstring;
      }
    }
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"object") == 0)
    {
      //return strdup("Object");
      return NULL;
    }
  }
  else
    return NULL;
}

int listsettings()
{
  char * returnstring = optionvalue(optionname, protoschema, protoface);
  if (returnstring)
  {
    puts(returnstring);
    free(returnstring);
  }
  else
    puts("undefined");
  return 1;
}

int setting(int argc, char *argv[])
{
  if (argc == 0)
  {
    return listsettings();
  }
  else if (argc == 1)
  {
    if (WJEBool(parameter,"readonly",WJE_GET,FALSE))
    {
      puts("option readonly");
      return 1;
    }
    return option_set_value(parameter, optionname, argv[0]);
  }
  else if (argc > 1)
  {
    if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0)
    {
      for (int i = 0; i < argc; i++)
      {
        option_set_value(parameter, optionname, argv[i]);
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
      return option_set_value(parameter, optionname, combine);
    }
  }
}

void listconditional(WJElement schema, WJElement face)
{
  WJElement anyoption = NULL;

  while (anyoption = _WJEObject(optionlist(schema, facename), "anyOf[]", WJE_GET, &anyoption)) {
    if (WJESchemaValidate(anyoption, protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      WJElement option = NULL;
      while ((option = _WJEObject(anyoption, "properties[]", WJE_GET, &option))) {
        printf("%s: \n", option->name);
      }
    }
  }
}

int isoptionconditional(WJElement schema, WJElement face, char * optionname)
{
  WJElement anyoption = NULL;
  while (anyoption = _WJEObject(optionlist(schema, facename), "anyOf[]", WJE_GET, &anyoption)) {
    if (WJESchemaValidate(anyoption, protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      if (WJEGetF(anyoption, NULL, "properties.%s", optionname))
        return 1;
    }
  }
  return 0;
}

int option_set_value(WJElement parameter, char * parametername, char * value)
{
  WJElement tempproto = WJEObject(NULL, NULL, WJE_NEW);
  WJECopyDocument(tempproto, WJEGet(protojson,"data",NULL), NULL, NULL);
  WJElement temp = WJEGet(tempproto,protoface->name,NULL);

  if (value[0] == '-' && strlen(value) == 1)
  {
    WJECloseDocument(WJEGet(temp,parametername,NULL));
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0)
  {
    if (value[0] == '-')
    {
      WJECloseDocument(WJEGetF(temp,NULL,"%s[] == %s",parametername,&value[1]));
    }
    else
    {
      if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"string") == 0){
        WJEStringF(temp, WJE_NEW, NULL, value, "%s[$]", parametername);
      }
      else if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"number") == 0){
        WJEInt32F(temp, WJE_NEW, NULL, atoi(value), "%s[$]", parametername);
      }
    }
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"string") == 0)
  {
    if (value[0] == '\"')
    {
      value = cutquot(value);
    }
    WJEString(temp, parametername, WJE_SET, value);
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"number") == 0)
    WJEInt32(temp, parametername, WJE_SET, atoi(value));
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"boolean") == 0)
  {
    if (strcmp(value, "true") == 0)
      WJEBool(temp, parametername, WJE_SET, TRUE);
    else if (strcmp(value, "false") == 0)
      WJEBool(temp, parametername, WJE_SET, FALSE);
    else
    {
      puts("value incorrect must be true or false");
      return 1;
    }
  }
  else
  {
    puts("Not implemeted");
    return 1;
  }

  if (WJESchemaValidate(optionlist(protoschema, protoface->name), temp, schema_error, schema_load, schema_free, "%s"))
  {
    WJElement parent = WJEGet(protoface->parent,"",NULL);
    WJECloseDocument(protoface);

    if (WJEGet(protojson, "schema.patternProperties", NULL))
    {
      WJEAttach(protojson,tempproto);
      protoface = WJEGet(tempproto,protoface->name,NULL);
      if (!protoface)
      {
        puts("FATAL ERROR");
        exit(0);
      }
    }
    else
    {
      WJEAttach(protojson,tempproto);
      protoface = WJEObject(protojson, "data", WJE_GET);
    }
    if (WJEGet(protojson,"schema.onset.command", NULL))
    {
      struct stat filestat;
      char onsetcommand[MAXPATH] = "\0";
      strcpy(onsetcommand,WJEString(protojson, "schema.onset.command", WJE_GET, NULL));
      if (stat(onsetcommand,&filestat))
      {
        onsetcommand[0] = '\0';
        strcat(onsetcommand, config.scriptpath);
        strcat(onsetcommand, "/");
        strcat(onsetcommand, WJEString(protojson, "schema.onset.command", WJE_GET, NULL));
      }
      if (stat(onsetcommand,&filestat))
      {
        puts("onset script inaccessible");
      }
      else
      {
        char *args[100];
        args[0] = onsetcommand;
        char * optionstring = optionvalue(parametername, protoschema, protoface);

        int argsc = arguments(WJEArray(protojson, "schema.onset.args", WJE_GET),args);

        char combinedepth[1000]; // replace with asprintf && free

       if (optiondepth > 0)
       {
         args[argsc++] = protoface->parent->name;
         snprintf(combinedepth,1000,"%s.%s", protoface->name, parametername);
         args[argsc++] = combinedepth;
       }
       else
       {
          args[argsc++] = protoface->name;
          args[argsc++] = parametername;
       }

        args[argsc++] = optionstring;
        args[argsc++] = NULL;

        if (WJEBool(protojson, "schema.onset.merge", WJE_GET, FALSE) == TRUE)
        {
          puts("merging");
          WJElement mergedata = streamfromcommand(onsetcommand,args,NULL);
          if (mergedata)
          {
            WJEDump(mergedata);
            WJEMergeObjects(WJEGet(protojson,"data",NULL), mergedata, TRUE);
          }
          else
            puts("no data");
          WJEDump(WJEGet(protojson,"data",NULL));
        }
        else
        {
          forkwaitexec(onsetcommand,argsc,args,NULL);
        }
        free(optionstring);
      }
    }
  }
  else
  {
    puts("Schema validation failed, check output below for mismatches");
    WJECloseDocument(tempproto);
  }
  return 1;
}

WJElement conditionoption(WJElement schema, WJElement face, char * optionname)
{
  char * facename = NULL;

  if (face)
    facename = face->name;
  else
    facename = NULL;

  if (!WJEGetF(optionlist(schema, facename), NULL, "properties.%s", optionname))
  {
    WJElement anyoption = NULL;
    while (anyoption = _WJEObject(schema, "anyOf[]", WJE_GET, &anyoption)) {
      if (WJESchemaValidate(anyoption, face, schema_errorq, schema_load, schema_free, "%s"))
      {
        return WJEObjectF(anyoption, WJE_GET, NULL, "properties.%s",optionname);
      }
    }
  }
  else
  {
    return WJEObjectF(optionlist(schema, facename), WJE_GET, NULL, "properties.%s",optionname);
  }
}

char * settingvalues(const char * text, int len, int state)
{
  static WJElement setting = NULL;
  if (strcmp(WJEString(rl_parameter,"type", WJE_GET, NULL),"boolean") == 0)
  {
    char *boolvariants[] = {"true","false",NULL};
    static int valueindex = 0;
    while (boolvariants[valueindex] != NULL)
    {
      if (strncmp(boolvariants[valueindex], text, len)==0)
      {
        return strdup(boolvariants[valueindex++]);
      }
      else
      {
        valueindex++;
      }
    }
    valueindex = 0;
  }
  else if (strcmp(WJEString(rl_parameter,"type", WJE_GET, NULL),"string") == 0)
  {
    char * variant = NULL;
    while (variant = _WJEString(rl_parameter, "enum[]", WJE_GET, &setting, NULL))
    {
      if (strncmp(variant, text, len) == 0) {
        return strdup(variant);
      }
    }
  }
  else if (strcmp(WJEString(rl_parameter,"type", WJE_GET, NULL),"number") == 0)
  {
    int number = 0;
    char cmpstr[10];
    while (number = _WJEInt32(rl_parameter, "enum[]", WJE_GET, &setting, 0))
    {
      sprintf(cmpstr,"%d",number);
      if (strncmp(cmpstr, text, len) == 0) {
        return strdup(cmpstr);
      }
    }
  }
  else if (strcmp(WJEString(rl_parameter,"type", WJE_GET, NULL),"array") == 0)
  {
    if (text[0] == '-')
    {
      /// SUDA
      static WJElement stashelement = NULL;
      if (strcmp(WJEString(WJEObject(rl_parameter, "items", WJE_GET),"type", WJE_GET, NULL),"number") == 0)
      {
        int somevalue = 0;
        char tempstring[100];
        while (somevalue = WJEInt32F(protoface, WJE_GET, &stashelement, 0, "%s[]", rl_parameter->name))
        {
          sprintf(tempstring,"-%d",somevalue);
          if (strncmp(tempstring, text, len) == 0) {
            return strdup(tempstring);
          }
        }
      }
      else if (strcmp(WJEString(WJEObject(rl_parameter, "items", WJE_GET),"type", WJE_GET, NULL),"string") == 0)
      {
        char * somestring = NULL;
        char tempstring[100];
        while (somestring = WJEStringF(protoface, WJE_GET, &stashelement, NULL, "%s[]", rl_parameter->name))
        {
          sprintf(tempstring,"-%s",somestring);
          if (strncmp(tempstring, text, len) == 0) {
            return strdup(tempstring);
          }
        }
      }
    }
    else
    {
      rl_parameter = WJEObject(rl_parameter, "items", WJE_GET);
      return settingvalues(text, len, state);
    }
  }
  return NULL;
}

char * singlecuevalue(const char * text, int len, char * cuename)
{
  static WJElement cueface = NULL;
  WJElement cueproto = WJEObject(root, cuename, WJE_GET);
  if (cuename)
  {
    while (cueface = WJEObjectF(root, WJE_GET, &cueface, "%s.data[]", cuename))
    {
      if (strncmp(elementname(cueproto,cueface), text, len) == 0) {
        return strdup(elementname(cueproto,cueface));
      }
    }
  }
  return NULL;
}

char * multicuevalue(const char * text, int len)
{
  static WJElement cue = NULL;
  static char * cuename = NULL;
  char * cuevalue = NULL;

  while (1)
  {
    cuevalue = singlecuevalue(text, len, cuename);
    if (cuevalue == NULL)
      cuename = _WJEString(rl_parameter, "cue[]", WJE_GET, &cue, NULL);
    if (cuevalue)
      return cuevalue;
    if (!cuevalue && !cuename)
      return NULL;
  }
}

char * cuesettingvalues(const char * text, int len, int state)
{
  if (strcmp(WJEString(rl_parameter,"type", WJE_GET, NULL),"string") == 0)
  {
      return multicuevalue(text,len);
  }
  else if (strcmp(WJEString(rl_parameter,"type", WJE_GET, NULL),"array") == 0)
  {
    rl_parameter = WJEObject(rl_parameter, "items", WJE_GET);
    return cuesettingvalues(text, len, state);
  }
  return NULL;
}
