#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>

char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

WJElement doc = NULL, schema = NULL, input = NULL, ifaceinput = NULL, ifaceoutput = NULL;
WJElement entity = NULL, parameter = NULL;

char greet[100] = ">";
char interface[100] = "";
char option[100] = "";
char *command;
int level = 0;

char *strmbtok ( char *input, char *delimit, char *openblock, char *closeblock) {
    static char *token = NULL;
    char *lead = NULL;
    char *block = NULL;
    int iBlock = 0;
    int iBlockIndex = 0;

    if ( input != NULL) {
        token = input;
        lead = input;
    }
    else {
        lead = token;
        if ( *token == '\0') {
            lead = NULL;
        }
    }

    while ( *token != '\0') {
        if ( iBlock) {
            if ( closeblock[iBlockIndex] == *token) {
                iBlock = 0;
            }
            token++;
            continue;
        }
        if ( ( block = strchr ( openblock, *token)) != NULL) {
            iBlock = 1;
            iBlockIndex = block - openblock;
            token++;
            continue;
        }
        if ( strchr ( delimit, *token) != NULL) {
            *token = '\0';
            token++;
            break;
        }
        token++;
    }
    return lead;
}

WJElement getelementbynameprop(WJElement container, char * text)
{
  WJElement entity = NULL;
  while (entity = _WJEObject(container, "[]", WJE_GET, &entity)) {
    if (strcmp(WJEString(entity, "name", WJE_GET, ""), text) == 0) {
      return entity;
    }
  }
  return NULL;
}

char * cutquot(char * stringtocut)
{
  stringtocut[strlen(stringtocut)-1] = '\0';
  return stringtocut+1;
}

int setparameter(char * setiface, char * setparam, char * setvalue)
{
  char temp[100];
  printf("setting %s %s %s\n",setiface,setparam,setvalue);
  if (setvalue[0] == '\"')
    setvalue = cutquot(setvalue);
  entity = getelementbynameprop(doc,setiface);
  sprintf(temp,"items.properties.%s",setparam);
  parameter = WJEObject(schema, temp, WJE_GET);
  WJEString(entity, parameter->name, WJE_SET, setvalue);
}

int arrlength(char **array)
{
  int length = 0;
  while (array[length] != NULL)
    length++;
  return length;
}

int parse(char * stringtoparse, char **tokarr)
{
  char acOpen[]  = {"\"[<{"};
  char acClose[] = {"\"]>}"};
  int i = 0;
  tokarr[i] = stringtoparse;
  while ((tokarr[i] = strmbtok(tokarr[i], " ", acOpen, acClose)) != NULL)
    tokarr[++i] = NULL;
}

void entitylist(void)
{
    while (entity = _WJEObject(doc, "[]", WJE_GET, &entity)) {
      puts(WJEString(entity, "name", WJE_GET, ""));
    }
}

char * entityvalues(const char * text, int len)
{
  while (entity = _WJEObject(doc, "[]", WJE_GET, &entity)) {
    if (strncmp(WJEString(entity, "name", WJE_GET, ""), text, len) == 0) {
      return strdup(WJEString(entity, "name", WJE_GET, ""));
    }
  }
}

void parameterlist()
{
  while (parameter = _WJEObject(schema, "items.properties[]", WJE_GET, &parameter)) {
    puts(parameter->name);
  }
}

char * parametervalues(const char * text, int len)
{
  while (parameter = _WJEObject(schema, "items.properties[]", WJE_GET, &parameter)) {
    if (strncmp(parameter->name, text, len) == 0) {
      return strdup(parameter->name);
    }
  }
}

int getparameter(char * getiface, char * getparam)
{
  char temp[100];
  sprintf(temp,"items.properties.%s",getparam);
  parameter = WJEObject(schema, temp, WJE_GET);
  if (parameter)
  {
    entity = getelementbynameprop(doc,getiface);
    printf("Value: %s\n", WJEString(entity, parameter->name, WJE_GET, ""));
    return 0;
  }
  else
  {
    printf("No property %s defined\n",getparam);
    return 1;
  }
}

char * settingvalues(char * getparam, int state)
{
  if (!state)
  {
    entity = getelementbynameprop(doc,interface);
    char temp[100];
    sprintf(temp,"items.properties.%s",getparam);
    parameter = WJEObject(schema, temp, WJE_GET);
    char * testquote = WJEString(entity, parameter->name, WJE_GET, "");
    if (strchr(testquote,' '))
    {
      sprintf(temp,"\"%s\"", testquote);
      return strdup(temp);
    }
    else
      return strdup(WJEString(entity, parameter->name, WJE_GET, ""));
  }
  return NULL;
}

int execute(int argc, char *argv[])
{
  if (strcmp(argv[0],"..")==0){
    if (level > 0)
      level--;
    else
      printf("Already at the command root\n",argv[0]);
    return 0;
  }
    switch (level)
    {
      case 0:
        if (argv[0][0]=='?') {
          entitylist();
          return 0;
        }
        if (argc == 1) {
          if (getelementbynameprop(doc,argv[0]) != NULL)
            strcpy(interface,argv[0]);
          else {
            printf("No interface %s found\n",argv[0]);
            return 0;
          }
        }
        else if (argc == 2){
          if (argv[1][0]=='?') {
            parameterlist();
            return 0;
          }
          else {
            getparameter(argv[0],argv[1]);
            return 0;
          }
        }
        else {
          setparameter(argv[0],argv[1],argv[2]);
          return 0;
        }
      break;
      case 1:
        if (argv[0][0]=='?') {
          parameterlist();
          return 0;
        }
        if (argc == 1) {
          getparameter(interface,argv[0]);
        }
        else {
          setparameter(interface,argv[0],argv[1]);
        }
      break;
      case 2:
        setparameter(interface,option,argv[0]);
        return 0;
      break;
    }
    if (level < 1)
      level++;
    //sprintf(greet,"%s >",line);
}

int interpret(char * stringtointerpret)
{
  char *tokarr[100]; // maximum argument count
  parse(stringtointerpret, tokarr);
  int numberoftokens = arrlength(tokarr);
  //printf("number of tokens %d\n", numberoftokens);
  if (numberoftokens > 0)
    execute(numberoftokens,tokarr);
}

int main(int argc, char *argv[])
{
    rl_attempted_completion_function = character_name_completion;

    FILE *jsonstream, *schemafile;
    WJReader readjson, readschema;

    if (!(jsonstream = popen("./wrapper.sh", "r")))
    {
      puts("handle error");
      return 1;
    }

    if (!(schemafile = fopen(argv[1], "r")))
    {
      puts("schema not found");
      return 1;
    }

    if (!(readjson = WJROpenFILEDocument(jsonstream, NULL, 0)))
    {
      puts("json failed to open");
      return 1;
    }

    if (!(readschema = WJROpenFILEDocument(schemafile, NULL, 0)))
    {
      puts("schema failed to open");
      return 1;
    }

    input = WJEOpenDocument(readjson, NULL, NULL, NULL);

    doc = WJEArray(NULL, NULL, WJE_NEW);

    while (ifaceinput = _WJEObject(input,"values[]", WJE_GET, &ifaceinput))
    {
      ifaceoutput = WJEObject(doc, "interface", WJE_NEW);

      WJEString(ifaceoutput,"name",WJE_NEW,ifaceinput->name);

      char * macaddr = WJEString(ifaceinput,"macaddr",WJE_GET,"");
      if (macaddr[0])
      {
        WJEString(ifaceoutput,"macaddr",WJE_NEW,macaddr);
      }

      int defaultroute = WJEInt32(ifaceinput,"defaultroute",WJE_GET,-1);
      if (defaultroute != -1)
      {
        WJEBool(ifaceoutput,"defaultroute",WJE_NEW,(XplBool)defaultroute);
      }

      int metric = WJEInt32(ifaceinput,"metric",WJE_GET,-1);
      if (metric != -1)
      {
        WJEInt32(ifaceoutput,"metric",WJE_NEW,metric);
      }

    }



    schema = WJEOpenDocument(readschema, NULL, NULL, NULL);

    while (1)
    {
      char * input = readline(greet);
      if (!input)
        break;
      add_history(input);
      interpret(input);
      free(input);
    }

    return 0;
}

char * find_command(char **tokarr, int start)
{
  int numberoftokens = arrlength(tokarr);
  if (numberoftokens > 0)
  {
    if (start > 0)
      return tokarr[0];
  }
  return NULL;
}

char ** character_name_completion(const char *text, int start, int end)
{
    char *tokarr[100];
    char *inputline = strdup(rl_line_buffer);
    parse(inputline, tokarr);
    command = find_command(tokarr, start);
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, character_name_generator);
}

char * character_name_generator(const char *text, int state)
{
    static int list_index, len;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    switch (level)
    {
      case 0:
        if (command == NULL)
          return entityvalues(text,len);
        else
          return parametervalues(text,len);
      break;
      case 1:
        if (command == NULL)
          return parametervalues(text,len);
        else
          return settingvalues(command, state);
      break;
      case 2:
        return settingvalues(option, state);
      break;
    }

    return NULL;
}
