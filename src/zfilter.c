#include <stdio.h>
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

	root = WJEObject(NULL, NULL, WJE_NEW);
	readconfig();
	loadeveryschema(root,config.schemapath);
	WJReader readjson;
	if (!(readjson = WJROpenFILEDocument(stdin, NULL, 0))) {
    puts("json failed to open");
    return 1;
  }

	//WJElement input = WJEOpenDocument(readjson, NULL, NULL, NULL);
	//WJEDump(input);
	//WJElement doc = filter(input,root, argv[1]);
	//(doc);

	WJEDump(filter(WJEOpenDocument(readjson, NULL, NULL, NULL),root, argv[1]));

	return 0;
}