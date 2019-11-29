#include <stdio.h>
#include <wjelement.h>
#include <string.h>

struct globalcommand_struct
{
  char *commandname;
  int schemacount;
  WJElement * schemalink;
} * globalcommands = NULL;

int globalcommandcount = 0;

void addglobalcommand(WJElement loadroot, char * commandname)
{
  printf("add global command %s\n", commandname);

  for (int i = 0; i < globalcommandcount; i++)
  {
    if (strcmp(commandname,globalcommands[i].commandname) == 0)
    {
      puts("add schema to existing command");
      globalcommands[i].schemacount++;
      globalcommands[i].schemalink = realloc(globalcommands[i].schemalink, sizeof(WJElement *)*globalcommands[i].schemacount);
      globalcommands[globalcommandcount-1].schemalink[globalcommands[i].schemacount-1] = loadroot;
      return;
    }
  }

  puts("add new command");
  globalcommandcount++;
  globalcommands = realloc(globalcommands, sizeof(struct globalcommand_struct)*(globalcommandcount));
  globalcommands[globalcommandcount-1].commandname = commandname;
  globalcommands[globalcommandcount-1].schemalink = malloc(sizeof(WJElement *));
  globalcommands[globalcommandcount-1].schemalink[0] = loadroot;
  globalcommands[globalcommandcount-1].schemacount = 1;
}

void listglobals(int argc, char *argv[])
{
  puts("globals:");
  for (int i = 0; i < globalcommandcount; i++)
  {
    puts(globalcommands[i].commandname);
  }
}

int isglobal(char * globalname)
{
  for (int i = 0; i < globalcommandcount; i++)
  {
    if (strcmp(globalname,globalcommands[i].commandname) == 0)
      return 1;
  }
  return 0;
}

int global(int argc, char *argv[])
{
  puts("execute global command");
  return 0;
}
