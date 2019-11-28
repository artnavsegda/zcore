#include <stdio.h>
#include <wjelement.h>

struct globalcommand_struct
{
  char *commandname;
  WJElement * schemalink;
} * globalcommands = NULL;

void addglobalcommand(WJElement loadroot, char * commandname)
{
  printf("add global command %s\n", commandname);

//  for (int i = 0, globalcommands

  globalcommands = realloc(globalcommands, sizeof(struct globalcommand_struct));
  globalcommands->commandname = commandname;
}

