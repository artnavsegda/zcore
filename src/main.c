#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement schema = NULL, doc = NULL;

int main(int argc, char *argv[])
{
  FILE *jsonstream, *schemafile;
  WJReader readjson, readschema;

  if (argc < 1) {
    puts("Enter conf category");
    return 1;
  }

  char temp[100];
  sprintf(temp,"%s.schema.json",argv[1]);
  if (!(schemafile = fopen(temp, "r"))) {
    puts("cannot open schema file %s\n",temp);
    return 1;
  }
  sprintf(temp,"./%s.sh",argv[1]);
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
  doc = ethernet(readjson);

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
