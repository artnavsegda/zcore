#include <wjelement.h>
#include <wjreader.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>
#include <dirent.h>
#include <string.h>

static int fileselect(const struct dirent *entry)
{
  char *ptr = rindex(entry->d_name, '.');
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

WJElement loadscheme(char * pathtoload)
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
  return schemaroot;
}

int loadeveryscheme(WJElement loadroot, char * loadschemepath)
{
  char path[MAXPATHLEN];
  getcwd(path,MAXPATHLEN);
  struct dirent **dirs;

  //load schemes from root

  int n = scandir(loadschemepath,&dirs,fileselect,alphasort);
  if (n >= 0)
  {
    chdir(loadschemepath);
    for (int cnt = 0;cnt < n;++cnt)
    {
      //puts(dirs[cnt]->d_name);
      WJEAttach(loadroot,loadscheme(dirs[cnt]->d_name));
    }
  }
  else
    printf("Cannot find files in %s\n", loadschemepath);

  //recursively load shemes from every subdir
  n = scandir(loadschemepath,&dirs,dirselect,alphasort);

  if (n >= 0)
  { 
    for (int cnt = 0;cnt < n;++cnt)
    {
      //printf("subdir %s\n",(dirs[cnt]->d_name));
      loadeveryscheme(WJEObject(loadroot, dirs[cnt]->d_name, WJE_NEW), dirs[cnt]->d_name);
    }
  } 
  else
  {
//    printf("Cannot find dirs in %s\n", loadschemepath);
  }

  chdir(path);
}
