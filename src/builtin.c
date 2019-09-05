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

void path_up()
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
      domain = FACE;
    break;
  }
}

void commandlist()
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
}

void listbuiltins()
{
  puts("?");
  puts("..");
}

int isbuiltin(char * builtinname)
{
  if ((strcmp(builtinname,"?") == 0) || (strcmp(builtinname,"..") == 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int builtin(int argc, char *argv[])
{
  if (argv[0][0]=='?')
  {
    commandlist();
  }
  else if (strcmp(argv[0],"..") == 0)
  {
    path_up();
  }
}

