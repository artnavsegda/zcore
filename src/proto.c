#include <stdio.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"
#include "proto.h"
#include "face.h"
#include "completion.h"

//char protoname[100] = "";
int protodepth = 0;
WJElement protojson = NULL;

int rl_protodepth = 0;
WJElement rl_protojson = NULL;

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

int rl_isproto(char * rl_protoname)
{
  if (rl_domain == PROTO)
  {
    if (rl_protodepth == 0)
      rl_protojson = root;

    if (WJEGet(rl_protojson, rl_protoname, NULL))
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
      //strcpy(protoname,argv[i]);
      protodepth++;
      protojson = WJEObject(protojson, argv[i], WJE_GET);
      if (WJEGet(protojson, "schema", NULL))
      {
        domain = FACE;
      }
    }
    else if(isface(argv[i]))
    {
      return face(argc-i, &argv[i]);
    }
    else
    {
      printf("%s unavalible\n", argv[i]);
    }
  }
  return 0;
}

int rl_proto(int argc, char *argv[])
{
  for (int i = 0; i < argc; i++)
  {
    if (rl_isproto(argv[i]))
    {
      rl_protodepth++;
      rl_protojson = WJEObject(rl_protojson, argv[i], WJE_GET);
      if (WJEGet(rl_protojson, "schema", NULL))
      {
        rl_domain = FACE;
      }
    }
    else if(rl_isface(argv[i]))
    {
      rl_face(argc-i, &argv[i]);
    }
  }
}

char * protovalues(const char * text, int len)
{
  if (rl_protodepth == 0)
    rl_protojson = root;
    
  static WJElement proto = NULL;

  while (proto = _WJEObject(rl_protojson, "[]", WJE_GET, &proto)) {
    if (strncmp(proto->name, text, len) == 0) {
      return strdup(proto->name);
    }
  }
  return NULL;
}

void incom_proto(void)
{
//  puts("proto completition init");
  rl_protojson = protojson;
  rl_protodepth = protodepth;
}

