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
  WJElement command = WJEObjectF(protojson, WJE_GET, NULL, "schema.commands.%s", argv[0]);

  if (WJEBool(command, "argpass", WJE_GET, 0) == TRUE)
  {
    arguments(WJEArray(command, "args", WJE_GET),&argv[argc]);
  }
  else
  {
    arguments(WJEArray(command, "args", WJE_GET),argv);
  }

  if (strcmp(WJEString(command,"json", WJE_GET, "none"),"out") == 0)
  {
    streamfromcommand(WJEString(command, "command", WJE_GET, "/bin/false"),argv,WJEArray(protojson, "data", WJE_GET));
  }
  else if (strcmp(WJEString(command,"json", WJE_GET, "none"),"in") == 0)
  {
    if (domain == OPTION)
    {
      streamintocommand(WJEString(command, "command", WJE_GET, "/bin/false"),argv,WJEToString(protoface,TRUE));
    }
    else if (domain == FACE)
    {
      WJElement face = NULL;
      while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
        streamintocommand(WJEString(command, "command" ,WJE_GET, "/bin/false"),argv,WJEToString(face,TRUE));
      }
    }
    else
    {
      puts("not implemented");
    }
  }
  else
  {
    if (WJEBool(command, "wait", WJE_GET, 0) == TRUE)
    {
      forkwaitexec(WJEString(command, "command", WJE_GET, "/bin/false"),argc,argv);
    }
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
