#include <stdio.h>
#include <string.h>
#include "builtin.h"
#include "interpreter.h"
#include "proto.h"
#include "zcore.h"
#include "face.h"
#include "command.h"
#include "option.h"

extern int protodepth;
extern WJElement protojson; 
extern WJElement protoface; 

int path_up()
{
  switch (domain)
  {
    case PROTO:
      if (protodepth == 0)
      {
        printf("Already at the command root\n");
      }
      else
      {
        protodepth--;
        protojson = protojson->parent;
      }
    break;
    case FACE:
      domain = PROTO;
      protodepth--;
      protojson = protojson->parent;
    break;
    case OPTION:
      if (strcmp(WJEString(protojson,"schema.type",WJE_GET,"unknown"),"array") == 0)
      {
        domain = FACE;
      }
      else if (strcmp(WJEString(protojson,"schema.type",WJE_GET,"unknown"),"object") == 0)
      {
        domain = PROTO;
        protodepth--;
        protojson = protojson->parent;
      }
    break;
  }
  return 0;
}

int commandlist()
{
  puts("tokens:");
  listbuiltins();
  switch (domain)
  {
    case PROTO:
      listprotos();
    break;
    case FACE:
      listfaces();
      listcommands();
    break;
    case OPTION:
      listoptions();
      listcommands();
    break;
  }
  return 1;
}

void listbuiltins()
{
  puts("?");
  puts("..");
  puts("show");
}

int isbuiltin(char * builtinname)
{
  if ((strcmp(builtinname,"?") == 0) || (strcmp(builtinname,"..") == 0) || (strcmp(builtinname,"show") == 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int builtin_show(int argc, char *argv[])
{
  WJElement option = NULL;
  if (domain == OPTION)
  {
    while (option = _WJEObject(optionlist(protojson), "properties[]", WJE_GET, &option)) {
      printf("%s.", WJEString(protoface, "name", WJE_GET, ""));
      printf("%s = ", option->name);
      printf("%s\n", WJEString(protoface, option->name, WJE_GET, "None"));
    }
  }
  else if (domain == FACE)
  {
    WJElement face = NULL;
    while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
      while (option = _WJEObject(optionlist(protojson), "properties[]", WJE_GET, &option)) {
        printf("%s.", WJEString(face, "name", WJE_GET, ""));
        printf("%s = ", option->name);
        printf("%s\n", WJEString(face, option->name, WJE_GET, "None"));
      }
    }
  }
  else
    puts("Not implemented");
}

int builtin(int argc, char *argv[])
{
  if (argv[0][0]=='?')
  {
    return commandlist();
  }
  else if (strcmp(argv[0],"..") == 0)
  {
    return path_up();
  }
  else if (strcmp(argv[0],"show") == 0)
  {
    return builtin_show(argc,argv);
  }
}

char *builtincommands[] = {"?","..","show",NULL};

char * builtinvalues(const char * text, int len)
{
  static int valueindex = 0;
  
  while (builtincommands[valueindex] != NULL)
  {
    if (strncmp(builtincommands[valueindex], text, len)==0)
    {
      return strdup(builtincommands[valueindex++]);
    }
    else
    {
      valueindex++;
    }
  }
  valueindex = 0;
  return NULL;
}

