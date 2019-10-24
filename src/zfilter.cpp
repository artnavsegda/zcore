#include <stdio.h>
#include <unistd.h>
#include <json-c/json.h>
#include "filter.h"
#include "config.h"
#include "load.h"

json_object * root = NULL;

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		puts("syntax: zfilter schema.path");
		return 1;
	}

	root = json_object_new_object();
	readconfig();
	loadeveryschema(root,config.schemapath);

  json_object * readjson = NULL;
  if ((readjson = json_object_from_fd(STDIN_FILENO)) == NULL) {
    puts("json failed to open");
    return 1;
  }

  puts(json_object_to_json_string_ext(filter(readjson,root, argv[1]), JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));

	return 0;
}
