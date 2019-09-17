#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include "filter.h"
#include "config.h"
#include "load.h"

WJElement doc = NULL;
WJElement root = NULL;

int main(int argc, char *argv[])
{
//  fprintf(stderr,"argumets %d\n",argc);
//  fprintf(stderr,"zero %s\n",argv[0]);
//  fprintf(stderr,"one %s\n",argv[1]);
//  fprintf(stderr,"two %s\n",argv[2]);

  setuid(0);
  root = WJEObject(NULL, NULL, WJE_NEW);

  readconfig();
  loadeveryschema(root,config.schemapath);

  FILE *jsonstream;
  WJReader readjson;
  char temp[100];
  if (!(jsonstream = popen(argv[2], "r"))) {
    puts("handle error");
    return 1;
  }

  if (!(readjson = WJROpenFILEDocument(jsonstream, NULL, 0))) {
    puts("json failed to open");
    return 1;
  }

  doc = filter(readjson,root, argv[1]);
  WJEDump(doc);

  return 0;
}
