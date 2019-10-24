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

	json_object * doc = NULL;
	// doc = generator(root, argv[optind], values, valindex);
	//puts(json_object_to_json_string_ext(doc, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));

	return 0;
}
