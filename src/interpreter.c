#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include "utils.h"
#include "interpreter.h"

//extern WJElement doc, schema;
//static WJElement entity = NULL, parameter = NULL, command = NULL;

//char greet[100] = ">";
//char interface[100] = "";
//char option[100] = "";
//int level = 0;

/*

int setparameter(char * setiface, char * setparam, char * setvalue) {
  printf("setting %s %s %s\n",setiface,setparam,setvalue);
  if (setvalue[0] == '\"')
    setvalue = cutquot(setvalue);
  entity = getelementbynameprop(doc,setiface);
  parameter = WJEObjectF(schema, WJE_GET, NULL, "items.properties.%s",setparam);
  WJEString(entity, parameter->name, WJE_SET, setvalue);
}

void entitylist(void) {
    while (entity = _WJEObject(doc, "[]", WJE_GET, &entity)) {
      puts(WJEString(entity, "name", WJE_GET, ""));
    }
}

void parameterlist() {
  while (parameter = _WJEObject(schema, "items.properties[]", WJE_GET, &parameter)) {
    puts(parameter->name);
  }
}

int getparameter(char * getiface, char * getparam) {
  parameter = WJEObjectF(schema, WJE_GET, NULL, "items.properties.%s",getparam);
  if (parameter) {
    entity = getelementbynameprop(doc,getiface);
    printf("Value: %s\n", WJEString(entity, parameter->name, WJE_GET, "<undefined>"));
    return 0;
  }
  else {
    printf("No property %s defined\n",getparam);
    return 1;
  }
}

*/

void commandlist() {
  puts("commands:");
  listbuiltins();
  listprotos();
  listcommands();
  listfaces();
  listoptions;
}

int execute(int argc, char *argv[]) {
  if (argv[0][0]=='?')
  {
    commandlist();
  }
  else if (isbuiltin(argv[0]))
  {
    builtin(argc,argv);
  }
  else if (isproto(argv[0]))
  {
    proto(argc,argv);
  }
  else if (iscommand(argv[0]))
  {
    command(argc,argv);
  }
  else if (isface(argv[0]))
  {
    face(argc,argv);
  }
  else if (isoption(argv[0]))
  {
    option(argc,argv);
  }
}

int interpret(char * stringtointerpret)
{
  char *tokarr[100]; // maximum argument count
  parse(stringtointerpret, tokarr);
  int numberoftokens = arrlength(tokarr);
  if (numberoftokens > 0)
  {
    execute(numberoftokens,tokarr);
  }
}
