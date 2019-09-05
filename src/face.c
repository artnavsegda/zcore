#include <stdio.h>
#include "zcore.h"
#include "interpreter.h"

extern WJElement protojson;

int isface(char * facename)
{
  if (domain == FACE)
  {

  }
  return 0;
}

int face(int argc, char *argv[])
{
}

int listfaces(void)
{
  WJElement face = NULL;
  puts("Faces:");
  while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
    puts(WJEString(face, "name", WJE_GET, ""));
  }
}

