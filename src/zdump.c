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

  while ((opt = getopt(argc, argv, "o:")) != -1)
  {
    switch (opt)
    {
      case 'o':
        break;
    }
  }

  root = WJEObject(NULL, NULL, WJE_NEW);
  readconfig(NULL);
  loadeveryschema(root,config.schemapath);

  WJEDump(root);

	return 0;
}
