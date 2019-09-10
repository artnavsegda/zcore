#include <stdio.h>
#include <wjelement.h>

int acquire(WJElement proto)
{
  //WJEDump(proto);
  //puts(WJEString(proto, "schema.acquire.shell", WJE_GET, "echo"));
  FILE *jsonstream;
  WJReader readjson;

  if (!(jsonstream = popen(WJEString(proto, "schema.acquire.shell", WJE_GET, "echo"), "r"))) {
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
}

int acquireall(WJElement directory)
{
  WJElement proto = NULL;
  while((proto = _WJEObject(directory, "[]", WJE_GET, &proto)))
  {
    if (WJEGet(proto, "schema", NULL))
    {
      //printf("acquire %s\n",proto->name);
      acquire(proto);
    }
    else
    {
      //printf("submodule %s\n",proto->name);
      acquireall(proto);
    }
  }
}
