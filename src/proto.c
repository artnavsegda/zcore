#include <stdio.h>
#include <string.h>
#include "interpreter.h"
#include "proto.h"
#include "face.h"
#include "completion.h"
#include "option.h"
#include "zcore.h"
#include "command.h"
#include "builtin.h"

int protodepth = 0;
WJElement protojson = NULL;

int rl_protodepth = 0;
WJElement rl_protojson = NULL;

extern WJElement protoface;
extern WJElement protoschema;
extern WJElement rl_protoschema;
extern WJElement rl_protoface;
extern enum domains domain;

int listprotos(void)
{
  if (protodepth == 0)
    protojson = root;

  WJElement proto = NULL;

  puts("Protos:");
  while ((proto = _WJEObject(protojson, "[]", WJE_GET, &proto))) {
    if (!WJEBool(proto, "schema.hidden", WJE_GET, FALSE))
    {
      //printf("%s: %s\n", proto->name, protohelp(proto->name));
      printf("%s\n", proto->name);
    }
  }
  return 0;
}

int isproto(char * protoname)
{
  if (domain == PROTO)
  {
    if (protodepth == 0)
      protojson = root;

    if (WJEChild(protojson, protoname, WJE_GET))
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

    if (WJEChild(rl_protojson, rl_protoname, WJE_GET))
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
    // if (argv[i][0] == '?')
    // {
    //   puts("Display description here");
    //   puts(protohelp("ethernet"));
    //   return 1;
    // }
    if (isproto(argv[i]))
    {
      //strcpy(protoname,argv[i]);
      protodepth++;
      protojson = WJEObject(protojson, argv[i], WJE_GET);
      if (WJEGet(protojson, "schema", NULL))
      {
        protoschema = WJEObject(protojson, "schema", WJE_GET);
        if (WJEGet(protoschema, "patternProperties", NULL))
        {
          domain = FACE;
        }
        else
        {
          domain = OPTION;
          protoface = WJEObject(protojson, "data", WJE_GET);
        }
      }
    }
    else if(isface(argv[i]))
    {
      return face(argc-i, &argv[i]);
    }
    else if(isoption(argv[i]))
    {
      return option(argc-i, &argv[i]);
    }
    else if(iscommand(argv[i]))
    {
      return command(argc-i, &argv[i]);
    }
    else if(isbuiltin(argv[i]))
    {
      return builtin(argc-i, &argv[i]);
    }
    else
    {
      printf("%s unavalible\n", argv[i]);
      return 1;
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
        rl_protoschema = WJEObject(rl_protojson, "schema", WJE_GET);
        if (WJEGet(rl_protojson, "schema.patternProperties", NULL))
        {
          rl_domain = FACE;
        }
        else if (WJEGet(rl_protojson, "schema.properties", NULL))
        {
          rl_protoface = WJEObject(rl_protojson, "data", WJE_GET);
          rl_domain = OPTION;
        }
      }
    }
    else if(rl_isface(argv[i]))
    {
      rl_face(argc-i, &argv[i]);
    }
    else if(rl_isoption(argv[i]))
    {
      rl_option(argc-i, &argv[i]);
    }
    else if(rl_iscommand(argv[i]))
    {
      rl_command(argc-i, &argv[i]);
    }
  }
  return 0;
}

char * protovalues(const char * text, int len)
{
  if (rl_protodepth == 0)
    rl_protojson = root;

  static WJElement proto = NULL;

  while (proto = _WJEObject(rl_protojson, "[]", WJE_GET, &proto)) {
    if (WJEBool(proto, "schema.hidden", WJE_GET, FALSE))
      return protovalues(text,len);
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
  rl_protoschema = protoschema;
}

char * protohelp(const char * commandname, WJElement proto)
{
  //WJEDump(proto);
  //puts(commandname);
  //WJElement proto = WJEObject(proto, commandname, WJE_GET);
  if (WJEGetF(proto, NULL, "%s.schema.description", commandname))
  {
    //puts("found");
    //puts(WJEStringF(proto, WJE_GET, NULL, NULL, "%s.schema.description", commandname));
    return strdup(WJEStringF(proto, WJE_GET, NULL, NULL, "%s.schema.description", commandname));
  }
  else
  {
    //puts("not found");
    return NULL;
  }
  //return "Help description";
}
