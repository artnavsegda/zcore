#include <stdio.h>
#include <unistd.h>
#include <sys/param.h>
#include <dirent.h>
#include <string.h>
#include <json-c/json.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

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

int loadeveryschema(json_object * loadroot, char * loadschemapath)
{
  char path[MAXPATHLEN];
  getcwd(path,MAXPATHLEN);
  struct dirent **dirs;

  int n = scandir(loadschemapath,&dirs,fileselect,alphasort);
  if (n >= 0)
  {
    chdir(loadschemapath);
    for (int cnt = 0;cnt < n;++cnt)
    {
      //puts(dirs[cnt]->d_name);
      json_object * schema = json_object_new_object();

      //json_object_object_add(loadroot,"schema", schema);
      json_object_object_add(loadroot,dirs[cnt]->d_name, schema);

      //json_object_object_add(schema,dirs[cnt]->d_name, json_object_from_file(dirs[cnt]->d_name));
      json_object_object_add(schema,"schema", json_object_from_file(dirs[cnt]->d_name));

      //json_object_object_add(loadroot,dirs[cnt]->d_name, json_object_from_file(dirs[cnt]->d_name));
    }
  }
  else
    printf("Cannot find files in %s\n", loadschemapath);

  n = scandir(loadschemapath,&dirs,dirselect,alphasort);

  if (n >= 0)
  {
    for (int cnt = 0;cnt < n;++cnt)
    {
      //printf("subdir %s\n",(dirs[cnt]->d_name));
      json_object * subroot = json_object_new_object();
      loadeveryschema(subroot,dirs[cnt]->d_name);
      json_object_object_add(loadroot, dirs[cnt]->d_name,subroot);
    }
  }
  else
  {
//    printf("Cannot find dirs in %s\n", loadschemapath);
  }

  chdir(path);
}
