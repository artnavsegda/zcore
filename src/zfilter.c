#include <stdio.h>
#include <unistd.h>
#include <wjelement.h>
#include <wjreader.h>
#include "filter.h"
#include "config.h"
#include "load.h"

WJElement root = NULL;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		puts("syntax: zfilter schema.path");
		return 1;
	}

	int opt;
	char *zcoreconfig = NULL;

	while ((opt = getopt(argc, argv, "c:")) != -1)
  {
    switch (opt)
    {
			case 'c': // config
	      zcoreconfig = optarg;
	      break;
    }
  }

	root = WJEObject(NULL, NULL, WJE_NEW);
	readconfig(zcoreconfig);
	loadeveryschema(root,config.schemapath);
	WJReader readjson;
	if (!(readjson = WJROpenFILEDocument(stdin, NULL, 0))) {
    puts("json failed to open");
    return 1;
  }

	WJEDump(filter(WJEOpenDocument(readjson, NULL, NULL, NULL),root, argv[optind]));

	return 0;
}
