#include <stdio.h>
#include <wjelement.h>
#include <wjreader.h>
#include <string.h>
#include "config.h"

struct configstruct_t config;

void parseconfig(WJElement configjson)
{
  strcpy(config.schemepath, WJEString(configjson, "name", WJE_GET, DEFAULTSCHEMEPATH));
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

  //puts(config.schemepath);

}

int list(const char *name, const struct stat *status, int type)
{
  puts(name);
  return 0;
}

int listschemefiles(char * searchpath)
{
  ftw(searchpath, listcb, 1);
}
