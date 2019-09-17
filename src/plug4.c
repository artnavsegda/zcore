#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include "generator.h"
#include "config.h"
#include "load.h"
#include "filter.h"

WJElement doc = NULL;
WJElement doc2 = NULL;
WJElement root = NULL;

int main(int argc, char *argv[])
{
  setuid(0);
  root = WJEObject(NULL, NULL, WJE_NEW);

  readconfig();
  loadeveryschema(root,config.schemapath);

  if (argc < 4)
  {
    puts("command schema script name");
    return 1;
  }

  doc = generator(argv[3], root, argv[1]);
  WJEDump(doc);

  doc2 = filter(doc, root, argv[1]);

  FILE *jsonstream;

  if (!(jsonstream = popen(argv[2], "w"))) {
    puts("handle error");
    return 1;
  }

  WJEWriteFILE(doc2, jsonstream);

  return 0;
}
