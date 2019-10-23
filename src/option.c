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
extern enum domains domain;
WJElement rl_parameter;

int listoptions(void)
{
  WJElement option = NULL;
  puts("Options:");

  while ((option = _WJEObject(optionlist(protojson), "properties[]", WJE_GET, &option))) {
    if (!WJEBool(option, "hidden", WJE_GET, FALSE))
      puts(option->name);
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
    else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"number") == 0){
      // to do
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
  if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"string") == 0)
  {
    if (value[0] == '\"')
    {
      value = cutquot(value);
    }
    WJEString(protoface, parameter->name, WJE_SET, value);
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"number") == 0)
    WJEInt32(protoface, parameter->name, WJE_SET, atoi(value));
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"boolean") == 0)
    WJEBool(protoface, parameter->name, WJE_SET, atoi(value));
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0)
  {
    if (value[0] == '-')
      WJEDettach(WJEGetF(protoface,NULL,"%s[] == %s",parameter->name,&value[1]));
    else
      WJEStringF(protoface, WJE_NEW, NULL, value, "%s[$]", parameter->name);
  }
  else
    puts("Not implemeted");
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
  if (argc == 1)
  {
    return option_print_value(parameter);
  }
  else if (argc == 2)
  {
    return option_set_value(parameter, argv[1]);
  }
  else if (argc > 2)
  {
    char combine[1000] = "";
    strcpy(combine, argv[1]);
    for (int i = 2; i < argc; i++)
    {
      strcat(combine, " ");
      strcat(combine, argv[i]);
    }
//    puts(combine);
    return option_set_value(parameter, combine);
//    return option_set_value(parameter,combinevalues(argc-1, &argv[1]));
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
  if (strcmp(WJEString(rl_parameter,"type", WJE_GET, NULL),"string") == 0)
  {
    static WJElement setting = NULL;
    char * variant = NULL;
    while (variant = _WJEString(rl_parameter, "enum[]", WJE_GET, &setting, NULL))
    {
      if (strncmp(variant, text, len) == 0) {
        return strdup(variant);
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
  if (cuename)
  {
    while (cueface = WJEObjectF(root, WJE_GET, &cueface, "%s.data[]", cuename))
    {
      if (strncmp(WJEString(cueface, "name", WJE_GET, ""), text, len) == 0) {
        return strdup(WJEString(cueface, "name", WJE_GET, ""));
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

