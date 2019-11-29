#include <stdio.h>
#include <unistd.h>
#include <wjelement.h>
#include <wjreader.h>
#include "generator.h"
#include "config.h"
#include "load.h"

WJElement root = NULL;

int main(int argc, char *argv[])
{
  int opt;
  char *zcoreconfig = NULL;

  while ((opt = getopt(argc, argv, "c:")) != -1)
  {
    switch (opt)
    {
      case 'c':
        zcoreconfig = optarg;
        break;
    }
  }

  root = WJEObject(NULL, NULL, WJE_NEW);
  readconfig(zcoreconfig);
  loadeveryschema(root,config.schemapath);

  WJEDump(root);

	return 0;
}
