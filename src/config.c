#include <stdio.h>
#include <wjelement.h>
#include <wjreader.h>
#include <string.h>
#include "config.h"

struct configstruct_t config;

void parseconfig(WJElement configjson)
{
  strcpy(config.schemapath, WJEString(configjson, "schemapath", WJE_GET, DEFAULTSCHEMAPATH));
  strcpy(config.scriptpath, WJEString(configjson, "scriptpath", WJE_GET, DEFAULTSCRIPTPATH));
  strcpy(config.jsonpath, WJEString(configjson, "jsonpath", WJE_GET, DEFAULTJSONPATH));
}

int readconfig(char * configpath)
{
  if (!configpath)
  {
    configpath = getenv("_CONFIG");
    if (!configpath)
      configpath = CONFIGPATH;
  }

  FILE *configfile;
  WJReader configread;
  WJElement configjson = NULL;
  if ((configfile = fopen(configpath, "r")) && (configread = WJROpenFILEDocument(configfile, NULL, 0)))
    configjson = WJEOpenDocument(configread, NULL, NULL, NULL);
  else
    configjson = WJEObject(NULL, NULL, WJE_NEW);

  strcpy(config.configpath, configpath);
  parseconfig(configjson);
  return 0;
}
