#include <stdio.h>
#include <string.h>
#include "config.h"

struct configstruct_t config;

//void parseconfig(WJElement configjson)
//{
//  strcpy(config.schemapath, WJEString(configjson, "schemapath", WJE_GET, DEFAULTSCHEMAPATH));
//}

int readconfig(void)
{
  json_object * configjson = NULL;
  int configfile = open("test.json", O_RDONLY);
  if (configfile != -1)
  {
    struct stat st;
    fstat(configfile, &st);
    char *buffer = malloc(st.st_size);
    read(jsonfile, buffer, st.st_size);
    configjson = json_tokener_parse(buffer);
  }
  else
  {
    configjson = json_object_new_object();
  }
//  if ((configfile = fopen(CONFIGPATH, "r")) && (configread = WJROpenFILEDocument(configfile, NULL, 0)))
//    configjson = WJEOpenDocument(configread, NULL, NULL, NULL);
//  else
//    configjson = WJEObject(NULL, NULL, WJE_NEW);
//
//  parseconfig(configjson);
}

