#include <stdio.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"
#include "utils.h"
#include "option.h"
#include "completion.h"

extern WJElement protojson;
extern WJElement rl_protojson;
WJElement protoface = NULL;
WJElement rl_protoface = NULL;
char facename[100] = "";
char rl_facename[100] = "";

int isface(char * facename)
{
  if (domain == FACE)
  {
    if (getelementbynameprop(protojson, facename))
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

int rl_isface(char * facename)
{
  if (rl_domain == FACE)
  {
    if (getelementbynameprop(rl_protojson, facename))
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

int face(int argc, char *argv[])
{
  strcpy(facename,argv[0]);
  protoface = getelementbynameprop(protojson, facename);
  domain = OPTION;
  if (argc > 1)
  {
    if(isoption(argv[1]))
    {
      return option(argc-1, &argv[1]);
    }
  }
  return 0;
}

int rl_face(int argc, char *argv[])
{
  strcpy(rl_facename,argv[0]);
  rl_protoface = getelementbynameprop(rl_protojson, rl_facename);
  rl_domain = OPTION;
}

int listfaces(void)
{
  WJElement face = NULL;
  puts("Faces:");
  while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
    puts(WJEString(face, "name", WJE_GET, ""));
  }
}

char * facevalues(const char * text, int len)
{
  static WJElement face = NULL;
  while (face = _WJEObject(rl_protojson, "data[]", WJE_GET, &face)) {
    if (strncmp(WJEString(face, "name", WJE_GET, ""), text, len) == 0) {
      return strdup(WJEString(face, "name", WJE_GET, ""));
    }
  }
  return NULL;
}

