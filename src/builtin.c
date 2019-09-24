#include <stdio.h>
#include <string.h>
#include "builtin.h"
#include "interpreter.h"
#include "proto.h"
#include "zcore.h"
#include "face.h"
#include "command.h"
#include "option.h"
#include "prompt.h"
#include "acquire.h"

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
  puts("acquire");
}

int isbuiltin(char * builtinname)
{
  if ((strcmp(builtinname,"?") == 0) || (strcmp(builtinname,"..") == 0) || (strcmp(builtinname,"show") == 0) || (strcmp(builtinname,"acquire") == 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int printoption(WJElement proto, WJElement face, int depth)
{
  WJElement option = NULL;
  while (option = _WJEObject(optionlist(proto), "properties[]", WJE_GET, &option)) {
    for (int i = depth; i > 0; i--)
    {
      printf("%s.", parentname(proto, i));
    }
    if (strcmp(WJEString(proto,"schema.type",WJE_GET,"unknown"),"array") == 0)
      printf("%s.", WJEString(face, "name", WJE_GET, ""));
    printf("%s = ", option->name);

    if (WJEGet(face,option->name,NULL))
    {
    if (strcmp(WJEString(option,"type", WJE_GET, NULL),"string") == 0)
      printf("%s", WJEString(face, option->name, WJE_GET, "None"));
    else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"number") == 0)
      printf("%d", WJEInt32(face,option->name,WJE_GET,-1));
    else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"boolean") == 0)
    {
      if (WJEBool(face,option->name,WJE_GET,-1) == TRUE)
        printf("True");
      else if (WJEBool(face,option->name,WJE_GET,-1) == FALSE)
        printf("False");
      else
        printf("None");
    }
    else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"array") == 0)
    {
      WJElement array = NULL;
      char * entity = NULL;
      while (entity = WJEStringF(face, WJE_GET, &array, NULL, "%s[]", option->name))
      {
        printf("%s ", entity);
      }
    }
    }
    else
      printf("None");
    puts("");
  }
}

int printoption2(WJElement proto, int depth)
{
  WJElement face = NULL;
  while (face = _WJEObject(proto, "data[]", WJE_GET, &face)) {
    printoption(proto,face, depth);
  }
}

int printoption3(WJElement protoinput, int depth)
{
  WJElement proto = NULL;
  depth++;
  while ((proto = _WJEObject(protoinput, "[]", WJE_GET, &proto))) {
    if (WJEGet(proto, "schema", NULL))
    {
      if (strcmp(WJEString(proto,"schema.type",WJE_GET,"unknown"),"array") == 0)
      {
        printoption2(proto, depth);
      }
      else if (strcmp(WJEString(proto,"schema.type",WJE_GET,"unknown"),"object") == 0)
      {
        printoption(proto,WJEObject(proto, "data", WJE_GET),depth);
      }
    }
    else
    {
      printoption3(proto, depth);
    }
  }
}

int builtin_show(int argc, char *argv[])
{
  if (domain == OPTION)
  {
    printoption(protojson,protoface,protodepth);
  }
  else if (domain == FACE)
  {
    printoption2(protojson, protodepth);
  }
  else if (domain == PROTO)
  {
    if (protodepth == 0)
      protojson = root;
    printoption3(protojson,protodepth);
  }
  else
    puts("Not implemented");
}

int builtin_acquire(int argc, char *argv[])
{
  acquire(WJEObject(root, argv[1], WJE_GET));
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
  else if (strcmp(argv[0],"acquire") == 0)
  {
    return builtin_acquire(argc,argv);
  }
}

char *builtincommands[] = {"?","..","show","acquire",NULL};

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

