#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include "completion.h"
#include "utils.h"
#include "proto.h"
#include "builtin.h"
#include "command.h"
#include "face.h"
#include "option.h"
#include "interpreter.h"

enum domains rl_domain = PROTO;
char *rl_command = NULL;

int rl_execute(int argc, char *argv[])
{
  if (isbuiltin(argv[0]))
  {
    //builtin(argc,argv);
  }
  else if (isproto(argv[0]))
  {
    //proto(argc,argv);
  }
  else if (iscommand(argv[0]))
  {
    //command(argc,argv);
  }
  else if (isface(argv[0]))
  {
    //face(argc,argv);
  }
  else if (isoption(argv[0]))
  {
    //option(argc,argv);
  }
}

int rl_interpret(char * stringtointerpret, int start, int end)
{
  rl_domain = domain;
  char *rl_tokarr[100];
  int one = parse(stringtointerpret, rl_tokarr);
  int numberoftokens = arrlength(rl_tokarr);
  if (numberoftokens > 0)
  {
    if (start > 0)
    {
      rl_execute(numberoftokens,rl_tokarr);
      rl_command = rl_tokarr[0];
      return 0;
    }
  }
  rl_command = NULL;
}

char ** character_name_completion(const char *text, int start, int end)
{
  rl_interpret(strdup(rl_line_buffer),start,end);
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, character_name_generator);
}

char * rl_rootcommands(const char * text, int len)
{
  char * rootvalues = NULL;
  //rootvalues = protovalues(text,len);
  rootvalues = builtinvalues(text,len);
  if (rootvalues == NULL)
  {
    return NULL;
  }
  else
  {
    return rootvalues;
  }
}

char * character_name_generator(const char *text, int state)
{
  static int list_index, len;

  if (!state) {
      list_index = 0;
      len = strlen(text);
  }

  if (rl_command == NULL)
  {
    return rl_rootcommands(text,len);
  }

  return NULL;
}

