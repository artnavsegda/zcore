#include <stdio.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"
#include "proto.h"
#include "face.h"

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
  for (int i = 0; i < argc; i++)
  {
    if (isproto(argv[i]))
    {
      strcpy(protoname,argv[i]);
      protodepth++;
      protojson = WJEObject(protojson, argv[i], WJE_GET);
      if (WJEGet(protojson, "schema", NULL))
      {
        domain = FACE;
      }
    }
    else if(isface(argv[i]))
    {
      face(argc-i, &argv[i]);
      return 1;
    }
    else
    {
      printf("%s unavalible\n", argv[i]);
    }
  }
}

char * protovalues(const char * text, int len)
{
  if (protodepth == 0)
    protojson = root;
    
  static WJElement proto = NULL;

  while (proto = _WJEObject(protojson, "[]", WJE_GET, &proto)) {
    if (strncmp(proto->name, text, len) == 0) {
      return strdup(proto->name);
    }
  }
  return NULL;
}
