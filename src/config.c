#include <stdio.h>
#include <wjelement.h>
#include <wjreader.h>
#include <string.h>
#include "config.h"

struct configstruct_t config;

void parseconfig(WJElement configjson)
{
  strcpy(config.schemapath, WJEString(configjson, "schemapath", WJE_GET, DEFAULTSCHEMAPATH));
}

int readconfig(void)
{
  FILE *configfile;
  WJReader configread;
  WJElement configjson = NULL;
  if ((configfile = fopen(CONFIGPATH, "r")) && (configread = WJROpenFILEDocument(configfile, NULL, 0)))
    configjson = WJEOpenDocument(configread, NULL, NULL, NULL);
  else
    configjson = WJEObject(NULL, NULL, WJE_NEW);

  parseconfig(configjson);
  return 0;
}


