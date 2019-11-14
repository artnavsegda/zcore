#include <stdio.h>
#include <wjelement.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "utils.h"
#include "config.h"

int acquire(WJElement proto)
{
  struct stat filestat;
  int forkpid = 0, status;
  FILE *jsonstream = NULL;
  WJReader readjson;
  char *argv[100];
  int argc = arguments(WJEArray(proto, "schema.acquire.args", WJE_GET), argv);

  WJElement data = NULL;
  if (data = WJEGet(proto,"data",NULL))
  {
    WJECloseDocument(data);
  }

  printf("acquire %s\n", proto->name);

  if (WJEGet(proto, "schema.acquire.file", NULL))
  {
    char * pathtoload = WJEString(proto, "schema.acquire.file", WJE_GET, NULL);
    if (!(jsonstream = fopen(pathtoload, "r"))) {
      printf("cannot open json file %s\n", pathtoload);
      return 1;
    }

    // do something
}
  else if (WJEGet(proto, "schema.acquire.shell", NULL))
  {
    char pathtoload[MAXPATH];
    strcpy(pathtoload,WJEString(proto, "schema.acquire.file", WJE_GET, NULL));

    if (stat(pathtoload,&filestat))
    {
      //direct path not found, trying to catcenate
      pathtoload[0] = '\0';
      strcat(pathtoload, config.scriptpath);
      strcat(pathtoload, "/");
      strcat(pathtoload, WJEString(proto, "schema.acquire.file", WJE_GET, NULL));
    }

    if (!(jsonstream = my_popen_read(pathtoload, argv,  NULL, &forkpid))) {
      puts("handle error");
      return 1;
    }
  }

  if (jsonstream)
  {

  if (!(readjson = WJROpenFILEDocument(jsonstream, NULL, 0))) {
    puts("json failed to open");
    return 1;
  }

  WJElement jsondata = WJEOpenDocument(readjson, NULL, NULL, NULL);
  WJERename(jsondata,"data");
  WJEAttach(proto,jsondata);

  fclose(jsonstream);
  if (forkpid)
    waitpid(forkpid, &status, 0);
  }

//  WJEDump(jsondata);
  return 0;
}

int acquireall(WJElement directory)
{
  WJElement proto = NULL;
  while((proto = _WJEObject(directory, "[]", WJE_GET, &proto)))
  {
    if (WJEGet(proto, "schema", NULL))
    {
      acquire(proto);
    }
    else
    {
      acquireall(proto);
    }
  }
  return 0;
}
