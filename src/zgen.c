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
	if (argc < 2)
	{
		puts("syntax: zgen -o name=value -o name=value schema.path");
		return 1;
	}

  char * values[100];
  int valindex = 0;
  int opt;

  while ((opt = getopt(argc, argv, "o:")) != -1)
  {
    switch (opt)
    {
      case 'o':
        values[valindex++] = optarg;
        break;
    }
  }

  for (int i = 0; i < valindex; i++)
  {
    puts(values[i]);
  }

  if (!argv[optind])
  {
    puts("syntax: zgen -o name=value -o name=value schema.path");
  }

  root = WJEObject(NULL, NULL, WJE_NEW);
  readconfig();
  loadeveryschema(root,config.schemapath);

  WJElement doc = NULL;
  doc = generator(root, argv[optind], values, valindex);
  WJEDump(doc);
	
	return 0;
}
