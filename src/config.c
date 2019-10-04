#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <json-c/json.h>
#include "config.h"

struct configstruct_t config;

void parseconfig(json_object * configjson)
{
  json_object *schemapath = NULL;
  if(json_object_object_get_ex(configjson, "schemapath", &schemapath))
  {
    strcpy(config.schemapath, json_object_to_json_string(schemapath));
  }
  else
    strcpy(DEFAULTSCHEMAPATH, config.schemapath);
}

int readconfig(void)
{
  json_object * configjson = NULL;
  int configfile = open("test.json", O_RDONLY);
  if (configfile != -1)
  {
    struct stat st;
    fstat(configfile, &st);
    char *buffer = malloc(st.st_size);
    read(configfile, buffer, st.st_size);
    configjson = json_tokener_parse(buffer);
  }
  else
  {
    configjson = json_object_new_object();
  }

  parseconfig(configjson);
}

