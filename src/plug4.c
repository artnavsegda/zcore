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
#include "filter2.h"

WJElement doc = NULL;
WJElement doc2 = NULL;
WJElement root = NULL;

int main(int argc, char *argv[])
{
  int opt;
  char *ubusconfig = NULL, *ubustype = NULL, *schema = NULL, *execmd = NULL, *namecmd = NULL;

  while ((opt = getopt(argc, argv, "e:s:c:t:")) != -1)
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

  //if (argc < 4)
  //{
  //  puts("command schema script name");
  //  return 1;
  //}

  doc = generator(root, schema, namecmd, argc-optind, &argv[optind]);
  WJEDump(doc);

  doc2 = filter(doc, root, schema, ubustype, ubusconfig);

  FILE *jsonstream;

  if (!(jsonstream = popen(execmd, "w"))) {
    puts("handle error");
    return 1;
  }

  WJEWriteFILE(doc2, jsonstream);

  return 0;
}
