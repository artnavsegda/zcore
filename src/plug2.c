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
#include "filter2.h"
#include "config.h"
#include "load.h"
#include "utils.h"

WJElement doc = NULL;
WJElement root = NULL;

extern char **environ;

int main(int argc, char *argv[])
{
  int opt;
  char *ubusconfig = NULL, *ubustype = NULL, *schema = NULL, *execmd = NULL, *namecmd = NULL;

  while ((opt = getopt(argc, argv, "n:e:s:c:t:")) != -1)
  {
    switch (opt)
    {
      case 'n':
        namecmd = optarg;
        break;
      case 'e': // execmd
        execmd = optarg;
        break;
      case 's': // schema
        schema = optarg;
        break;
      case 'c': // config
        ubusconfig = optarg;
        break;
      case 't': // type
        ubustype = optarg;
        break;
    }
  }

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

  doc = filter(input, root, schema, ubustype, ubusconfig);

  FILE *jsonstream;

  if (!(jsonstream = my_popen_write(execmd, argv, environ))) {
    puts("handle error");
    return 1;
  }

//  WJEWriteFILE(doc, stdout);
  WJEWriteFILE(doc, jsonstream);

  return 0;
}
