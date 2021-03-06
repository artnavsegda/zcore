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
  WJElement option_parameter = NULL;
  char * returnstring = NULL;

  option_parameter = conditionoption(proto, face, commandname);

  //puts("OPTIONVALUE CONDITIONOPTION DUMP");
  //WJEDump(option_parameter);
  //puts("END OPTIONVALUE CONDITIONOPTION DUMP END");

  if (WJEGet(face, commandname, NULL))
  {
    if (strcmp(WJEString(option_parameter,"type", WJE_GET, NULL),"string") == 0){
      return strdup(WJEString(face,commandname,WJE_GET,"<undefined>"));
    }
    else if (strcmp(WJEString(option_parameter,"type", WJE_GET, NULL),"number") == 0){
      asprintf(&returnstring,"%d", WJEInt32(face,commandname,WJE_GET,-1));
      return returnstring;
    }
    else if (strcmp(WJEString(option_parameter,"type", WJE_GET, NULL),"boolean") == 0){
      if (WJEBool(face,commandname,WJE_GET,-1) == TRUE)
        return strdup("true");
      else if (WJEBool(face,commandname,WJE_GET,-1) == FALSE)
        return strdup("false");
    }
    else if (strcmp(WJEString(option_parameter,"type", WJE_GET, NULL),"array") == 0){
      WJElement array = NULL;
      if (strcmp(WJEString(option_parameter,"items.type", WJE_GET, NULL),"string") == 0){
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
      else if (strcmp(WJEString(option_parameter,"items.type", WJE_GET, NULL),"number") == 0){
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
    else if (strcmp(WJEString(option_parameter,"type", WJE_GET, NULL),"object") == 0)
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

int setvalue(WJElement parameter, char * parametername, char * value, WJElement temp)
{
  //set value
  if (value[0] == '-' && strlen(value) == 1)
  {
    WJECloseDocument(WJEGet(temp,parametername,NULL));
  }
  else if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0)
  {
    if (value[0] == '-')
    {
      printf("deleting %s\n",&value[1]);

      //WJEDump(WJEGetF(temp,NULL,"%s[] == %s",parametername,&value[1]));
      WJEDump(WJEGetF(temp,NULL,"%s[] == \"%s\"",parametername,&value[1]));

      WJECloseDocument(WJEGetF(temp,NULL,"%s[] == \"%s\"",parametername,&value[1]));
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

int setting(int argc, char *argv[])
{
  char * result = NULL;
  if (argc == 0)
  {
    return listsettings();
  }
  else
  {
    if (WJEBool(parameter,"readonly",WJE_GET,FALSE))
    {
      puts("option readonly");
      return 1;
    }
    WJElement tempproto = WJEObject(NULL, "data", WJE_NEW);
    WJECopyDocument(tempproto, WJEGet(protojson,"data",NULL), NULL, NULL);
    WJElement temp = WJEGet(tempproto,protoface->name,NULL);
    if (argc == 1)
    {
      setvalue(parameter, optionname, argv[0], temp);

      result = optionvalue(optionname, protoschema, temp);
      if (!result)
      {
        result = strdup("");
      }

      if (WJEBool(protojson, "schema.onset.merge", WJE_GET, FALSE) == TRUE)
        onset(optionname, tempproto, result);
      if (validate(temp, tempproto, optionname))
        if (WJEBool(protojson, "schema.onset.merge", WJE_GET, FALSE) == FALSE)
          onset(optionname, WJEGet(protojson,"data",NULL), result);

      free(result);
      return 1;
    }
    else if (argc > 1)
    {
      char combine[1000] = "";
      strcpy(combine, argv[1]);
      for (int i = 2; i < argc; i++)
      {
        strcat(combine, " ");
        strcat(combine, argv[i]);
      }
      if (strcmp(WJEString(parameter,"type", WJE_GET, NULL),"array") == 0)
      {
        for (int i = 0; i < argc; i++)
        {
          setvalue(parameter, optionname, argv[i], temp);
        }
      }
      else
      {
        setvalue(parameter, optionname, combine, temp);
      }

      result = optionvalue(optionname, protoschema, temp);

      if (WJEBool(protojson, "schema.onset.merge", WJE_GET, FALSE) == TRUE)
        onset(optionname, tempproto, result);
      if (validate(temp, tempproto, optionname))
        if (WJEBool(protojson, "schema.onset.merge", WJE_GET, FALSE) == FALSE)
          onset(optionname, WJEGet(protojson,"data",NULL), result);

      WJECloseDocument(tempproto);
      free(result);
      return 1;
    }
  }
}
