#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include "interpreter.h"
#include "completion.h"
#include "config.h"
#include "load.h"
#include "acquire.h"

WJElement root = NULL;

int main(int argc, char *argv[])
{
//  rl_attempted_completion_function = character_name_completion;

  root = WJEObject(NULL, NULL, WJE_NEW);

  readconfig();
//  printf("Schema search path %s\n",config.schemepath);
  loadeveryscheme(root,config.schemepath);

//  WJEDump(root);

  acquireall(root);

  return 0;

  /*FILE *jsonstream;
  WJReader readjson;
  sprintf(temp,"./%s ./%s.sh",argv[1],argv[1]);
  if (!(jsonstream = popen(temp, "r"))) {
    puts("handle error");
    return 1;
  }

  if (!(readjson = WJROpenFILEDocument(jsonstream, NULL, 0))) {
    puts("json failed to open");
    return 1;
  }
  if (!(readschema = WJROpenFILEDocument(schemafile, NULL, 0))) {
    puts("schema failed to open");
    return 1;
  }

  schema = WJEOpenDocument(readschema, NULL, NULL, NULL);
  doc = WJEOpenDocument(readjson, NULL, NULL, NULL);

  while (1)
  {
    char * input = readline(">");
    if (!input)
      break;
    add_history(input);
    interpret(input);
    free(input);
  }

  return 0;*/
}
