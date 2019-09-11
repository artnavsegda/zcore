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

  doc = filter(readjson, root, argv[1]);

//  WJWriter writejson;

//  if (!(writejson = WJWOpenFILEDocument(0, stdout))) {
//    puts("stream failed to open");
//    return 1;
//  }

  WJEWriteFILE(doc, stdout);

  //WJEDump(doc);

  return 0;
}
