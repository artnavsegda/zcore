#include <stdio.h>
#include <string.h>
#include "face.h"
#include "zcore.h"
#include "interpreter.h"
#include "utils.h"
#include "option.h"
#include "completion.h"
#include "command.h"
#include "builtin.h"

extern WJElement protojson;
extern WJElement rl_protojson;
extern WJElement rl_commjson;
extern enum domains domain;
WJElement protoface = NULL;
WJElement protoschema = NULL;
WJElement rl_protoface = NULL;
WJElement rl_protoschema = NULL;
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
    else if (iscommand(argv[1]))
    {
      return command(argc-1, &argv[1]);
    }
    else if(isbuiltin(argv[1]))
    {
      return builtin(argc-1, &argv[1]);
    }
  }
  return 0;
}

int rl_face(int argc, char *argv[])
{
  strcpy(rl_facename,argv[0]);
  rl_protoface = getelementbynameprop(rl_protojson, rl_facename);
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
    //printf("%s: %s\n", elementname(protojson,face), facehelp(NULL));
  }
  return 0;
}

char * facevalues(const char * text, int len)
{
  if (rl_domain == COMMAND && WJEGet(rl_commjson, "cue", NULL))
    return NULL;

  static WJElement face = NULL;
  while (face = _WJEObject(rl_protojson, "data[]", WJE_GET, &face)) {
    if (strncmp(elementname(rl_protojson,face), text, len) == 0) {
      return strdup(elementname(rl_protojson,face));
    }
  }
  return NULL;
}

char * facehelp(char * facename, WJElement proto)
{
  //puts(facename);
  //WJEDump(proto);

  WJElement face = getelementbynameprop(proto, facename);

  if (!face)
    return NULL;

  WJElement properties = optionlist(WJEGet(proto, "schema", NULL) , face->name);

  if (properties)
  {
    if (WJEGet(properties, "description", NULL))
      return strdup(WJEString(properties, "description", WJE_GET, NULL));
  }

  return NULL;
  //return WJEString(optionlist(protojson, protoface->name), "description", WJE_GET, NULL);
}
