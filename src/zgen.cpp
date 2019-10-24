#include <stdio.h>
#include <unistd.h>
#include <json-c/json.h>
// #include "generator.h"
#include "config.h"
#include "load.h"

json_object * root = NULL;

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

  if (!argv[optind])
  {
    puts("syntax: zgen -o name=value -o name=value schema.path");
  }

  root = json_object_new_object();
  readconfig();
  loadeveryschema(root,config.schemapath);

  // WJElement doc = NULL;
  // doc = generator(root, argv[optind], values, valindex);
  // WJEDump(doc);

	return 0;
}
