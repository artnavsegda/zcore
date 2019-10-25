#include <stdio.h>
#include <wjelement.h>
#include "utils.h"

int acquire(WJElement proto)
{
  FILE *jsonstream;
  WJReader readjson;
  char *argv[100];
  int argc = arguments(WJEArray(proto, "schema.acquire.args", WJE_GET), argv);

  WJElement data = NULL;
  if (data = WJEGet(proto,"data",NULL))
  {
    WJEDettach(data);
  }

  //printf("acquire %s\n", proto->name);
  //puts(WJEString(proto, "schema.acquire.shell", WJE_GET, "/bin/true"));

  if (!(jsonstream = my_popen_read(WJEString(proto, "schema.acquire.shell", WJE_GET, "/bin/true"), argv,  NULL))) {
    puts("handle error");
    return 1;
  }

  if (!(readjson = WJROpenFILEDocument(jsonstream, NULL, 0))) {
    puts("json failed to open");
    return 1;
  }

  WJElement jsondata = WJEOpenDocument(readjson, NULL, NULL, NULL);
  WJERename(jsondata,"data");
  WJEAttach(proto,jsondata);
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
