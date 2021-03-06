#include <wjelement.h>
#include <wjreader.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>
#include <dirent.h>
#include <string.h>
#ifndef STANDALONE
#include "global.h"
#endif

static int fileselect(const struct dirent *entry)
{
  char *ptr = rindex((char *)entry->d_name, '.');
  if ((ptr!=NULL) && ((strcmp(ptr,".json")==0)))
    return 1;
  else
    return 0;
}

static int dirselect(const struct dirent *entry)
{
  if (entry->d_type == DT_DIR)
    if (entry->d_name[0] != '.')
      return 1;
  return 0;
}

WJElement loadschema(char * pathtoload)
{
  FILE *schemafile;
  WJReader readschema;

  //printf("loading %s\n",pathtoload);

  if (!(schemafile = fopen(pathtoload, "r"))) {
    printf("cannot open schema file %s\n", pathtoload);
    return NULL;
  }

  if (!(readschema = WJROpenFILEDocument(schemafile, NULL, 0))) {
    puts("schema failed to open");
    return NULL;
  }

  WJElement schema = WJEOpenDocument(readschema, NULL, NULL, NULL);
  WJERename(schema,"schema");
  WJElement schemaroot = WJEObject(NULL,WJEString(schema, "title", WJE_GET, "unnamed"), WJE_NEW);
  WJEAttach(schemaroot,schema);

#ifndef STANDALONE

  WJElement command = NULL;
  while (command = _WJEObject(schemaroot, "schema.commands[]", WJE_GET, &command)) {
    if (WJEBool(command, "global", WJE_GET, FALSE))
    {
      //printf("adding global command %s from schema %s\n", command->name, schemaroot->name);
      addglobalcommand(schemaroot, command->name);
    }
  }

#endif

  return schemaroot;
}

int loadeveryschema(WJElement loadroot, char * loadschemapath)
{
  char path[MAXPATHLEN];
  getcwd(path,MAXPATHLEN);
  struct dirent **dirs;

  //printf("current path %s loading from %s\n", path, loadschemapath);

  //load schemas from root

  int n = scandir(loadschemapath,&dirs,fileselect,alphasort);
  if (n >= 0)
  {
    chdir(loadschemapath);
    for (int cnt = 0;cnt < n;++cnt)
    {
      //puts(dirs[cnt]->d_name);
      WJEAttach(loadroot,loadschema(dirs[cnt]->d_name));
    }
    chdir(path);
  }
  else
  {
    printf("Cannot find files in %s\n", loadschemapath);
  }

  //recursively load shemes from every subdir
  n = scandir(loadschemapath,&dirs,dirselect,alphasort);

  if (n >= 0)
  {
    for (int cnt = 0;cnt < n;++cnt)
    {
      //printf("subdir %s\n",(dirs[cnt]->d_name));
      chdir(loadschemapath);
      loadeveryschema(WJEObject(loadroot, dirs[cnt]->d_name, WJE_NEW), dirs[cnt]->d_name);
      chdir(path);
    }
  }
  else
  {
//    printf("Cannot find dirs in %s\n", loadschemapath);
  }

  //chdir(path);
  return 0;
}
