#include <stdio.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"
#include "utils.h"
#include "option.h"
#include "completion.h"
#include "command.h"

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
      printf("%s it is face\n", facename);
      return 1;
    }
    else
    {
      printf("%s it is not face\n", facename);
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
    else if (iscommand(argv[1]))
    {
      return command(argc-1, &argv[1]);
    }
  }
  return 0;
}

int rl_face(int argc, char *argv[])
{
  strcpy(rl_facename,argv[0]);
  rl_protoface = getelementbynameprop(rl_protojson, rl_facename);
  puts("switching to option\n");
  rl_domain = OPTION;
  if (argc > 1)
  {
    if(rl_isoption(argv[1]))
    {
      return rl_option(argc-1, &argv[1]);
    }
    else if (rl_iscommand(argv[1]))
    {
      return rl_command(argc-1, &argv[1]);
    }
  }
  return 0;
}

int listfaces(void)
{
  WJElement face = NULL;
  puts("Faces:");
  while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
    puts(elementname(protojson,face));
  }
}

char * facevalues(const char * text, int len)
{
  printf("facevalues %s %d", text, len);
  static WJElement face = NULL;
  while (face = _WJEObject(rl_protojson, "data[]", WJE_GET, &face)) {
    if (strncmp(elementname(rl_protojson,face), text, len) == 0) {
      return strdup(elementname(rl_protojson,face));
    }
  }
  return NULL;
}

