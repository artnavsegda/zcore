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

WJElement doc = NULL;
WJElement root = NULL;

int main(int argc, char *argv[])
{
  setuid(0);
  root = WJEObject(NULL, NULL, WJE_NEW);

  readconfig();
  loadeveryschema(root,config.schemapath);

  if (argc < 3)
  {
    puts("command schema name");
    return 1;
  }

  doc = generator(argv[2], root, argv[1]);
  WJEDump(doc);

  return 0;
}
