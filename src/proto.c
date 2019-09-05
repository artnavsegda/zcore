#include <stdio.h>
#include <string.h>
#include "zcore.h"

char protopath[100] = "";
int protodepth = 0;
WJElement protojson = NULL;

int listprotos(void)
{
  if (protodepth == 0)
    protojson = root;

  WJElement proto = NULL;

  puts("Protos:");
//  while ((proto = WJEObjectF(protojson, WJE_GET, &proto, "%s[]", protopath))) {
//    puts(proto->name);
//  }
  while ((proto = _WJEObject(protojson, "[]", WJE_GET, &proto))) {
    puts(proto->name);
  }
}

int isproto(char * protoname)
{
  if (protodepth == 0)
    protojson = root;

  //if (WJEGetF(root, NULL, "%s", protoname))
  if (WJEGet(protojson, protoname, NULL))
  {
    return 1;
    //puts("proto exists");
  }
  else
  {
    return 0;
    //puts("proto not exists");
  }
}

int proto(int argc, char *argv[])
{
  strcpy(protopath,argv[0]);
  protodepth++;
  protojson = WJEObject(protojson, argv[0], WJE_GET);
  if (WJEGet(protojson, "schema", NULL))
  {
    puts("proto is schema");
  }
  else
  {
    puts("proto is subdir");
  }
}

