#include <stdio.h>
#include <wjelement.h>
#include <string.h>
#include "domain.h"
#include "command.h"

extern WJElement protojson;
extern WJElement protoschema;
extern enum domains domain;

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
  for (int i = 0; i < globalcommandcount; i++)
  {
    if (strcmp(argv[0],globalcommands[i].commandname) == 0)
    {
      //printf("execute global command %s\n", globalcommands[i].commandname);
      //we have to stack all working variables
      WJElement stack_protojson = protojson;
      WJElement stack_protoschema = protoschema;
      enum domains stack_domain = domain;
      // set up fake enviroment
      domain = FACE; //global commands execute in face domain
      for (int y = 0; y < globalcommands[i].schemacount; y++)
      {
        //printf("schema for command %s\n", globalcommands[i].schemalink[y]->name);
        //printf("schema for command %s\n", WJEString(globalcommands[i].schemalink[y], "title", WJE_GET, "unknown"));
        //WJEDump(globalcommands[i].schemalink[y]);
        protojson = globalcommands[i].schemalink[y];
        command(argc, argv);
      }
      //stack back
      protojson = stack_protojson;
      protoschema = stack_protoschema;
      domain = stack_domain;
    }
  }

  return 0;
}

char * globalvalues(const char * text, int len)
{
  static int valueindex = 0;

  while (valueindex < globalcommandcount)
  {
    if (strncmp(globalcommands[valueindex].commandname, text, len)==0)
    {
      return strdup(globalcommands[valueindex++].commandname);
    }
    else
    {
      valueindex++;
    }
  }
  valueindex = 0;
  return NULL;
}
