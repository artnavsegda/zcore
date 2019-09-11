#include <stdio.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"
#include "command.h"
#include "utils.h"

extern WJElement protojson;
extern WJElement protoface;
extern WJElement rl_protojson;

int listcommands(void)
{
  WJElement command = NULL;
  puts("Commands:");

  while ((command = _WJEObject(protojson, "schema.commands[]", WJE_GET, &command))) {
    puts(command->name);
  }
}

int iscommand(char * commandname)
{
  if (domain == FACE || domain == OPTION)
  {
    if (WJEGetF(protojson, NULL, "schema.commands.%s", commandname))
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

int rl_iscommand(char * commandname)
{
  if (domain == FACE || domain == OPTION)
  {
    if (WJEGetF(rl_protojson, NULL, "schema.commands.%s", commandname))
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

int command(int argc, char *argv[])
{
  WJElement face = NULL;
  if (domain == OPTION)
  {
    streamintocommand(WJEStringF(protojson, WJE_GET, NULL, "not found","schema.commands.%s.command", argv[0]),WJEToString(protoface,TRUE),"");
  }
  else if (domain == FACE)
  {
    while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
      streamintocommand(WJEStringF(protojson, WJE_GET, NULL, "not found","schema.commands.%s.command", argv[0]),WJEToString(face,TRUE),"");
    }
  }
  else
  {
    puts("not implemented");
  }
}

char * commandvalues(const char * text, int len)
{
  static WJElement command = NULL;
  while (command = _WJEObject(rl_protojson, "schema.commands[]", WJE_GET, &command)) {
    if (strncmp(command->name, text, len) == 0) {
      return strdup(command->name);
    }
  }
  return NULL;
}

