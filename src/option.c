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

extern WJElement protojson;
extern WJElement protoschema;
extern WJElement protoface;
extern WJElement rl_protojson;
extern WJElement rl_protoschema;
extern WJElement rl_protoface;
extern char facename[100];
extern enum domains domain;
WJElement rl_parameter = NULL;
WJElement optionjson = NULL;
int optiondepth = 0;

void listconditional(WJElement schema, WJElement face)
{
  WJElement option = NULL;
  if (WJEGet(optionlist(schema, face->name), "if", NULL))
  {
    if (WJESchemaValidate(WJEGet(optionlist(schema, face->name), "if", NULL), protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      while ((option = _WJEObject(WJEGet(optionlist(schema, face->name), "then", NULL), "properties[]", WJE_GET, &option))) {
        printf("%s: \n", option->name);
      }
    }
    else
    {
      while ((option = _WJEObject(WJEGet(optionlist(schema, face->name), "else", NULL), "properties[]", WJE_GET, &option))) {
        printf("%s: \n", option->name);
      }
      listconditional(WJEGet(optionlist(schema, face->name), "else", NULL), face);
    }
  }
}

int listoptions(void)
{
  WJElement option = NULL;
  puts("Options:");

  //WJEDump(optionlist(protoschema));

  while ((option = _WJEObject(optionlist(protoschema, protoface->name), "properties[]", WJE_GET, &option))) {
    if (!WJEBool(option, "hidden", WJE_GET, FALSE))
    {
      printf("%s: ", option->name);
      //printf("%s: %s\n", option->name, WJEString(option, "description", WJE_GET, NULL));
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
      //puts(option->name);
    }
  }

  listconditional(protoschema, protoface);

  return 0;
}

int isoptionconditional(WJElement schema, WJElement face, char * optionname)
{
  if (WJEGet(optionlist(schema, face->name), "if", NULL))
  {
    if (WJESchemaValidate(WJEGet(optionlist(schema, face->name), "if", NULL), face, schema_errorq, schema_load, schema_free, "%s"))
    {
      if (WJEGetF(WJEGet(optionlist(schema, face->name), "then", NULL), NULL, "properties.%s", optionname))
        return 1;
    }
    else
    {
      //WJEDump(optionlist(schema, face->name));
      if (WJEGet(optionlist(schema, face->name), "else", NULL))
      {
        //puts("else condition found");
        if (WJEGetF(WJEGet(optionlist(schema, face->name), "else", NULL), NULL, "properties.%s", optionname))
          return 1;
        else
          return isoptionconditional(WJEGet(optionlist(schema, face->name), "else", NULL), face, optionname);
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
    else
    {
      return isoptionconditional(protoschema, protoface, optionname);
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
    else
    {
      return isoptionconditional(rl_protoschema, rl_protoface, optionname);
    }
  }
  return 0;
}

int ValidateConditional(WJElement schema, WJElement json)
{
  if (WJEGet(schema, "if", NULL))
  {
    //puts("if directive");
    //WJEDump(WJEGet(schema, "if", NULL));
    //puts("condition");
    //WJEDump(json);
    if (WJESchemaValidate(WJEGet(schema, "if", NULL), json, schema_errorq, schema_load, schema_free, "%s"))
    {
      //puts("true");
      if (WJESchemaValidate(WJEGet(schema, "then", NULL), json, schema_errorq, schema_load, schema_free, "%s"))
      {
        return ValidateConditional(WJEGet(schema, "then", NULL), json);
      }
      else
        return 0;
    }
    else if (WJESchemaValidate(WJEGet(schema, "else", NULL), json, schema_errorq, schema_load, schema_free, "%s"))
    {
      return ValidateConditional(WJEGet(schema, "else", NULL), json);
    }
    else
      return 0;
  }
  return 1;
}

int option_set_value(WJElement parameter, char * parametername, char * value)
{
  if (value[0] == '?')
  {
    puts(WJEString(parameter,"description",WJE_GET,"Help not found"));
    return 1;
  }
  else
  {
    WJElement temp = WJEObject(NULL, protoface->name, WJE_NEW);
    WJECopyDocument(temp, protoface, NULL, NULL);

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

    //WJEDump(temp);

    if (WJESchemaValidate(optionlist(protoschema, protoface->name), temp, schema_error, schema_load, schema_free, "%s") && ValidateConditional(optionlist(protoschema, protoface->name), temp))
    {
      //puts("schema valid");

      WJElement parent = WJEGet(protoface->parent,"",NULL);
      WJECloseDocument(protoface);

      if (WJEGet(protojson, "schema.patternProperties", NULL))
      {

        WJEAttach(parent,temp);
        protoface = WJEGet(temp,"",NULL);
        if (!protoface)
        {
          puts("FATAL ERROR");
          exit(0);
        }

        //protoface = getelementbynameprop(protojson, facename);
        //if (!protoface)
        //  domain = FACE;



      }
      else
      {
        WJEAttach(protojson,temp);
        protoface = WJEObject(protojson, "data", WJE_GET);
      }


      //puts(protoface->name);
      //protoface = temp;
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
           snprintf(combinedepth,1000,"%s.%s", protoface->parent->name, protoface->name);
           args[argsc++] = combinedepth;
         }
         else
         {
            args[argsc++] = protoface->name;
         }

          args[argsc++] = parametername;
          args[argsc++] = optionstring;
          args[argsc++] = NULL;

          if (WJEBool(protojson, "schema.onset.wait", WJE_GET, FALSE) == TRUE)
          {
            forkwaitexec(onsetcommand,argsc,args,NULL);
          }
          else
          {
            forkexec(onsetcommand,5,args,NULL);
          }
          free(optionstring);
        }
      }
    }
    else
    {
      puts("Schema validation failed, check output below for mismatches");
      WJECloseDocument(temp);
    }
  }
  return 1;
}

char * combinevalues(int argc, char *argv[])
{
  char * combine = "hello good sir";
  return combine;
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
    if (WJESchemaValidate(WJEGet(optionlist(schema, facename), "if", NULL), face, schema_errorq, schema_load, schema_free, "%s"))
    {
      return WJEObjectF(WJEGet(optionlist(schema, facename), "then", NULL), WJE_GET, NULL, "properties.%s",optionname);
    }
    else
    {
      return conditionoption(WJEGet(optionlist(schema, facename), "else", NULL), face, optionname);
    }
  }
  else
  {
    return WJEObjectF(optionlist(schema, facename), WJE_GET, NULL, "properties.%s",optionname);
  }
}

int option(int argc, char *argv[])
{
  WJElement parameter = NULL;

  //WJEDump(WJEGetF(optionlist(protoschema, protoface->name), NULL, "properties.%s", argv[0]));

  parameter = conditionoption(protoschema, protoface, argv[0]);

  if (WJEGet(parameter, "[\"$ref\"]", NULL))
  {
    parameter = WJEGetF(root, NULL, "%s.schema", WJEString(parameter, "[\"$ref\"]", WJE_GET, NULL));
  }

  //WJEDump(parameter);

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
  return 1;
}

int rl_option(int argc, char *argv[])
{
  rl_parameter = conditionoption(rl_protoschema, rl_protoface, argv[0]);

  //rl_parameter = WJEObjectF(optionlist(rl_protoschema, rl_protoface->name), WJE_GET, NULL, "properties.%s",argv[0]);

  if (WJEGet(rl_parameter, "[\"$ref\"]", NULL))
  {
    rl_parameter = WJEGetF(root, NULL, "%s.schema", WJEString(rl_parameter, "[\"$ref\"]", WJE_GET, NULL));
  }

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

char * conditionvalues(WJElement schema, WJElement face, const char * text, int len)
{
  static WJElement option = NULL;
  if (WJEGet(optionlist(schema, face->name), "if", NULL))
  {
    if (WJESchemaValidate(WJEGet(optionlist(schema, face->name), "if", NULL), protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      while ((option = _WJEObject(WJEGet(optionlist(schema, face->name), "then", NULL), "properties[]", WJE_GET, &option)))
      {
        if (strncmp(option->name, text, len) == 0)
        {
          return strdup(option->name);
        }
      }
    }
    else
    {
      if (WJEGet(optionlist(schema, face->name), "else.properties", NULL))
      {
        while ((option = _WJEObject(WJEGet(optionlist(schema, face->name), "else", NULL), "properties[]", WJE_GET, &option)))
        {
          if (strncmp(option->name, text, len) == 0)
          {
            return strdup(option->name);
          }
        }
        return NULL;
      }
      else
        return conditionvalues(WJEGet(optionlist(schema, face->name), "else", NULL), face, text, len);
    }
  }
  return NULL;
}

char * conoptionvalues(const char * text, int len)
{
  return conditionvalues(rl_protoschema, rl_protoface, text, len);
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
    rl_parameter = WJEObject(rl_parameter, "items", WJE_GET);
    return settingvalues(text, len, state);
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
