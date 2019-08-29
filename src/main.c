#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include "interpreter.h"
#include "completion.h"

WJElement schema = NULL, doc = NULL;

int main(int argc, char *argv[])
{
  rl_attempted_completion_function = character_name_completion;

  FILE *jsonstream, *schemafile;
  WJReader readjson, readschema;

  if (argc < 2) {
    puts("Enter proto type");
    return 1;
  }

  char temp[100];
  sprintf(temp,"json/%s.schema.json",argv[1]);
  if (!(schemafile = fopen(temp, "r"))) {
    printf("cannot open schema file %s\n",temp);
    return 1;
  }

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

  return 0;
}
