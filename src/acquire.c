#include <stdio.h>
#include <wjelement.h>

int acquire(WJElement proto)
{
  //WJEDump(proto);
  puts(WJEString(proto, "schema.acquire.shell", WJE_GET, "echo"));


  //FILE *jsonstream;

  //if (!(jsonstream = popen(WJEString(proto, "scheme.acquire.shell", WJE_GET, "echo"), "r"))) {
  //}


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

