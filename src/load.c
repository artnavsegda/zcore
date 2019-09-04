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

  printf("loading %s\n",pathtoload);

  if (!(schemafile = fopen(pathtoload, "r"))) {
    printf("cannot open schema file %s\n", pathtoload);
    return NULL;
  }

  if (!(readschema = WJROpenFILEDocument(schemafile, NULL, 0))) {
    puts("schema failed to open");
    return NULL;
  }

  WJElement schema = WJEOpenDocument(readschema, NULL, NULL, NULL);
  WJERename(schema,WJEString(schema, "title", WJE_GET, "default"));
  return schema;
}

int loadeveryscheme(WJElement loadroot, char * loadschemepath)
{
  struct dirent **dirs;

  //load schemes from root

  int n = scandir(loadschemepath,&dirs,fileselect,alphasort);
  if (n >= 0)
  {
    chdir(loadschemepath);
    for (int cnt = 0;cnt < n;++cnt)
    {
      WJEAttach(loadroot,loadscheme(dirs[cnt]->d_name));
    }
  }
  else
    printf("Cannot open directory %s for files\n", loadschemepath);

  //recursively load shemes from every subdir
  n = scandir(loadschemepath,&dirs,dirselect,alphasort);

  if (n >= 0)
  { 
    for (int cnt = 0;cnt < n;++cnt)
    {
      puts(dirs[cnt]->d_name);
      loadeveryscheme(loadroot, dirs[cnt]->d_name);
    }
  } 
  else
    printf("Cannot open directory %s for files\n", loadschemepath);
}
