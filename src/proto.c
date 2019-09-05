#include <stdio.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"

//char protopath[100] = "";
char protoname[100] = "";
int protodepth = 0;
WJElement protojson = NULL;

int listprotos(void)
{
  if (protodepth == 0)
    protojson = root;

  WJElement proto = NULL;

  puts("Protos:");
  while ((proto = _WJEObject(protojson, "[]", WJE_GET, &proto))) {
    puts(proto->name);
  }
}

int isproto(char * protoname)
{
  if (domain == PROTO)
  {
    if (protodepth == 0)
      protojson = root;

    if (WJEGet(protojson, protoname, NULL))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

int proto(int argc, char *argv[])
{
//  strcpy(protopath,argv[0]);
  strcpy(protoname,argv[0]);
  protodepth++;
  protojson = WJEObject(protojson, argv[0], WJE_GET);
  if (WJEGet(protojson, "schema", NULL))
  {
    //puts("proto is schema");
    domain = FACE;
  }
  else
  {
    //puts("proto is subdir");
  }
}

