#include <stdio.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"
#include "utils.h"

extern WJElement protojson;
WJElement protoface = NULL;
char facename[100] = "";

int isface(char * facename)
{
  if (domain == FACE)
  {
    if (getelementbynameprop(protojson, facename))
    {
      //puts(facename);
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

int face(int argc, char *argv[])
{
  strcpy(facename,argv[0]);
  protoface = getelementbynameprop(protojson, facename);
  domain = OPTION;
}

int listfaces(void)
{
  WJElement face = NULL;
  puts("Faces:");
  while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
    puts(WJEString(face, "name", WJE_GET, ""));
  }
}

