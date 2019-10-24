#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
// #include "zcore.h"
#include "interpreter.h"
#include "face.h"
#include "utils.h"
#include "option.h"
// #include "completion.h"
// #include "command.h"

extern enum domains rl_domain;

// extern WJElement rl_protojson;
json_object * protoface = NULL;
json_object * rl_protoface = NULL;
extern json_object * protojson;
extern json_object * rl_protojson;
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
//   if (argc > 1)
//   {
//     if(isoption(argv[1]))
//     {
//       return option(argc-1, &argv[1]);
//     }
//     else if (iscommand(argv[1]))
//     {
//       return command(argc-1, &argv[1]);
//     }
//   }
  return 0;
}

int rl_face(int argc, char *argv[])
{
  strcpy(rl_facename,argv[0]);
  rl_protoface = getelementbynameprop(rl_protojson, rl_facename);
  rl_domain = OPTION;
  // if (argc > 1)
  // {
  //   if(rl_isoption(argv[1]))
  //   {
  //     return rl_option(argc-1, &argv[1]);
  //   }
  // }
  return 0;
}

int listfaces(void)
{
  json_object * faces = NULL;

  if (json_object_object_get_ex(protojson, "data", &faces))
  {
    puts("Faces:");
    json_object_object_foreach(faces, key, val)
    {
      puts(key);
    }
  }

//   while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
//     puts(WJEString(face, "name", WJE_GET, ""));
//   }
  return 0;
}

// char * facevalues(const char * text, int len)
// {
//   static WJElement face = NULL;
//   while (face = _WJEObject(rl_protojson, "data[]", WJE_GET, &face)) {
//     if (strncmp(WJEString(face, "name", WJE_GET, ""), text, len) == 0) {
//       return strdup(WJEString(face, "name", WJE_GET, ""));
//     }
//   }
//   return NULL;
// }
