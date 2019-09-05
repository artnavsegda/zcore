#include <stdio.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"
#include "command.h"

extern WJElement protojson;

int listcommands(void)
{
  WJElement command = NULL;
  puts("Commands:");

  while ((command = _WJEObject(protojson, "schema.commands[]", WJE_GET, &command))) {
    puts(command->name);
  }
}

