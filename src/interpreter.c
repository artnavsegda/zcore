#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include "utils.h"

WJElement doc = NULL, schema = NULL, input = NULL, ifaceinput = NULL, ifaceoutput = NULL;
WJElement entity = NULL, parameter = NULL;

char greet[100] = ">";
char interface[100] = "";
char option[100] = "";
char *command;
int level = 0;

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

void entitylist(void)
{
    while (entity = _WJEObject(doc, "[]", WJE_GET, &entity)) {
      puts(WJEString(entity, "name", WJE_GET, ""));
    }
}

void parameterlist()
{
  while (parameter = _WJEObject(schema, "items.properties[]", WJE_GET, &parameter)) {
    puts(parameter->name);
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
