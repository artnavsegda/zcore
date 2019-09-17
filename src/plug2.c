#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include <wjwriter.h>
#include <wjreader.h>
#include <unistd.h>
#include "filter.h"
#include "config.h"
#include "load.h"

WJElement doc = NULL;
WJElement root = NULL;

int main(int argc, char *argv[])
{
  setuid(0);
  root = WJEObject(NULL, NULL, WJE_NEW);

  readconfig();
  loadeveryschema(root,config.schemapath);

  WJReader readjson;

  if (!(readjson = WJROpenFILEDocument(stdin, NULL, 0))) {
    puts("json failed to open");
    return 1;
  }

  WJElement input = WJEOpenDocument(readjson, NULL, NULL, NULL);

  doc = filter(input, root, argv[1]);

  FILE *jsonstream;

  if (!(jsonstream = popen(argv[2], "w"))) {
    puts("handle error");
    return 1;
  }

//  WJEWriteFILE(doc, stdout);
  WJEWriteFILE(doc, jsonstream);

  return 0;
}
