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
    strcpy(config.schemapath, json_object_get_string(schemapath));
  }
  else
    strcpy(DEFAULTSCHEMAPATH, config.schemapath);
}

int readconfig(void)
{
  json_object * configjson = json_object_from_file(CONFIGPATH);
  if (configjson == NULL)
  {
    configjson = json_object_new_object();
  }
  parseconfig(configjson);
  return 0;
}

