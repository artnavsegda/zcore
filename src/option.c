#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <wjelement.h>
#include "zcore.h"
#include "interpreter.h"
#include "option.h"
#include "utils.h"
#include "completion.h"
#include "builtin.h"

extern WJElement protojson;
extern WJElement protoface;
extern WJElement rl_protojson;
extern WJElement rl_protoface;
extern enum domains domain;
WJElement rl_parameter = NULL;
WJElement optionjson = NULL;
int optiondepth = 0;

int listoptions(void)
{
  WJElement option = NULL;
  puts("Options:");

  while ((option = _WJEObject(optionlist(protojson), "properties[]", WJE_GET, &option))) {
    if (!WJEBool(option, "hidden", WJE_GET, FALSE))
    {
      printf("%s: ", option->name);
      //printf("%s: %s\n", option->name, WJEString(option, "description", WJE_GET, NULL));
      char * optionstring = optionvalue(option->name, protojson, protoface);
      if (optionstring)
      {
        printf("%s\n", optionstring);
        free(optionstring);
      }
      else
      {
        printf("None\n", optionstring);
      }
      //puts(option->name);
    }
  }
  return 0;
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

int option_print_value(WJElement parameter)
{
  if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"string") == 0){
    puts(WJEString(protoface,parameter->name,WJE_GET,"<undefined>"));
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"number") == 0){
    printf("%d\n", WJEInt32(protoface,parameter->name,WJE_GET,-1));
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"boolean") == 0){
    if (WJEBool(protoface,parameter->name,WJE_GET,-1) == TRUE)
      puts("True");
    else if (WJEBool(protoface,parameter->name,WJE_GET,-1) == FALSE)
      puts("False");
    else
      puts("<undefined>");
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0){
    WJElement array = NULL;
    if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"string") == 0){
      char * entity = NULL;
      while (entity = WJEStringF(protoface, WJE_GET, &array, NULL, "%s[]", parameter->name))
        puts(entity);
    }
    else if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"number") == 0){
      int number = 0;
      while (number = WJEInt32F(protoface, WJE_GET, &array, 0, "%s[]", parameter->name))
        printf("%d ", number);
      puts("");
    }
  }
  else
  {
    puts("Not implemeted");
  }
  return 1;
}

int option_set_value(WJElement parameter, char * value)
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

    if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0)
    {
      if (value[0] == '-')
      {
        WJECloseDocument(WJEGetF(temp,NULL,"%s[] == %s",parameter->name,&value[1]));
      }
      else
      {
        if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"string") == 0){
          WJEStringF(temp, WJE_NEW, NULL, value, "%s[$]", parameter->name);
        }
        else if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"number") == 0){
          WJEInt32F(temp, WJE_NEW, NULL, atoi(value), "%s[$]", parameter->name);
        }
      }
    }
    else if (value[0] == '-')
    {
      WJECloseDocument(WJEGet(temp,parameter->name,NULL));
    }
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"string") == 0)
    {
      if (value[0] == '\"')
      {
        value = cutquot(value);
      }
      WJEString(temp, parameter->name, WJE_SET, value);
    }
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"number") == 0)
      WJEInt32(temp, parameter->name, WJE_SET, atoi(value));
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"boolean") == 0)
      WJEBool(temp, parameter->name, WJE_SET, atoi(value));
    else
      puts("Not implemeted");

    if (WJESchemaValidate(optionlist(protojson), temp, schema_error, schema_load, schema_free, NULL))
    {
      //puts("schema valid");
      WJECloseDocument(protoface);
      WJEAttach(WJEGet(protojson,"data",NULL),temp);
      protoface = temp;
    }
    else
    {
      //puts("schema invalid");
    }
  }
  return 1;
}

char * combinevalues(int argc, char *argv[])
{
  char * combine = "hello good sir";
  return combine;
}

int option(int argc, char *argv[])
{
  WJElement parameter;
  parameter = WJEObjectF(optionlist(protojson), WJE_GET, NULL, "properties.%s",argv[0]);

  if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"object") == 0)
  {
    //puts("going down");
    protoface = WJEGet(protoface,argv[0],NULL);
    optiondepth++;
    return 0;
  }

  if (argc == 1)
  {
    char * returnstring = optionvalue(argv[0], protojson, protoface);
    if (returnstring)
    {
      puts(returnstring);
      free(returnstring);
    }
    else
      puts("undefined");
    return 1;
    //return option_print_value(parameter);
  }
  else if (argc == 2)
  {
    if (WJEBool(parameter,"readonly",WJE_GET,FALSE))
    {
      puts("option readonly");
      return 1;
    }
    return option_set_value(parameter, argv[1]);
  }
  else if (argc > 2)
  {
    if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0)
    {
      for (int i = 1; i < argc; i++)
      {
        option_set_value(parameter, argv[i]);
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
      return option_set_value(parameter, combine);
    }
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
    if (WJEBool(option, "hidden", WJE_GET, FALSE))
      return optionvalues(text,len);
    if (strncmp(option->name, text, len) == 0) {
      return strdup(option->name);
    }
  }
  return NULL;
}

char * settingvalues(const char * text, int len, int state)
{
  static WJElement setting = NULL;
  if (strcmp(WJEString(rl_parameter,"type", WJE_GET, NULL),"string") == 0)
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

char * optionhelp(const char * commandname)
{
  //WJElement proto = WJEObject(protojson, commandname, WJE_GET);
  //if (proto)
  //{
  //  return WJEString(proto, "schema.description", WJE_GET, NULL);
  //}
  return WJEStringF(optionlist(rl_protojson), WJE_GET, NULL, NULL, "properties.%s.description", commandname);
  //return "Help description";
}

char * optionvalue(const char * commandname, WJElement proto, WJElement face)
{
  WJElement parameter;
  char * returnstring = NULL;
  parameter = WJEObjectF(optionlist(proto), WJE_GET, NULL, "properties.%s",commandname);
  if (WJEGet(face, parameter->name, NULL))
  {
    if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"string") == 0){
      return strdup(WJEString(face,parameter->name,WJE_GET,"<undefined>"));
    }
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"number") == 0){
      asprintf(&returnstring,"%d", WJEInt32(face,parameter->name,WJE_GET,-1));
      return returnstring;
    }
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"boolean") == 0){
      if (WJEBool(face,parameter->name,WJE_GET,-1) == TRUE)
        return strdup("True");
      else if (WJEBool(face,parameter->name,WJE_GET,-1) == FALSE)
        return strdup("False");
    }
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0){
      WJElement array = NULL;
      if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"string") == 0){
        char * entity = NULL;
        returnstring = malloc(1);
        returnstring[0] = '\0';
        while (entity = WJEStringF(face, WJE_GET, &array, NULL, "%s[]", parameter->name))
        {
          //puts(entity);
          realloc(returnstring, strlen(returnstring) + strlen(entity) + 1);
          strcat(returnstring, entity);
          strcat(returnstring, " ");
        }
        return returnstring;
      }
      else if (strcmp(WJEString(parameter,"items.type", WJE_GET, NULL),"number") == 0){
        int number = 0;
        returnstring = malloc(1);
        returnstring[0] = '\0';
        while (number = WJEInt32F(face, WJE_GET, &array, 0, "%s[]", parameter->name))
        {
          sprintf(returnstring, "%s%d ", returnstring, number);
        }
        return returnstring;
      }
    }
  }
  else
    return NULL;
}
