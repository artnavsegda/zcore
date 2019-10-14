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
char *rl_commandname = NULL;

int rl_execute(int argc, char *argv[])
{
  if (isbuiltin(argv[0]))
  {
    //builtin(argc,argv);
  }
  else if (rl_isproto(argv[0]))
  {
    rl_proto(argc,argv);
  }
  else if (rl_iscommand(argv[0]))
  {
    rl_command(argc,argv);
  }
  else if (rl_isface(argv[0]))
  {
    rl_face(argc,argv);
  }
  else if (rl_isoption(argv[0]))
  {
    rl_option(argc,argv);
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
      rl_commandname = rl_tokarr[0];
      return 0;
    }
  }
  rl_commandname = NULL;
}

void init_completition(void)
{
  incom_proto();
}

char ** character_name_completion(const char *text, int start, int end)
{
  init_completition();
  rl_interpret(strdup(rl_line_buffer),start,end);
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, character_name_generator);
}

char * rl_rootcommands(const char * text, int len)
{
  char * rootvalues = NULL;
  if ((rootvalues = builtinvalues(text,len)) == NULL)
  {
    switch (domain)
    {
      case PROTO:
        rootvalues = protovalues(text,len);
      break;
      case FACE:
        if ((rootvalues = facevalues(text,len)) == NULL)
          rootvalues = commandvalues(text,len);
      break;
      case OPTION:
        if ((rootvalues = optionvalues(text,len)) == NULL)
          rootvalues = commandvalues(text,len);
      break;
    }
  }
  return rootvalues;
}

char * rl_subcommands(const char * text, int len, int state)
{
  char * subvalues = NULL;
  switch (rl_domain)
  {
    case PROTO:
    if ((subvalues = protovalues(text,len)) == NULL)
      return NULL;
    break;
    case FACE:
    if ((subvalues = facevalues(text,len)) == NULL)
      return NULL;
    break;
    case OPTION:
    if ((subvalues = optionvalues(text,len)) == NULL)
      subvalues = commandvalues(text,len);
    break;
    case SETTING:
    if ((subvalues = settingvalues(text,len,state)) == NULL)
     subvalues = cuesettingvalues(text,len,state);
//    if ((subvalues = cuesettingvalues(text,len,state)) == NULL)
//      return NULL;
    break;
    case COMMAND:
    if ((subvalues = cuecommandvalues(text,len,state)) == NULL)
      return NULL;
    break;
  }
  return subvalues;
}

char * character_name_generator(const char *text, int state)
{
  static int list_index, len;

  if (!state) {
      list_index = 0;
      len = strlen(text);
  }

  if (rl_commandname == NULL)
  {
    return rl_rootcommands(text,len);
  }
  else
  {
    return rl_subcommands(text,len, state);
  }

  return NULL;
}

int zc_completion(int count, int key)
{
  char **something;
  something = rl_completion_matches("s", character_name_generator);
  if (something)
  {
    printf("\nhello kids\n");
    int i = 0;
    while (something[i])
      puts(something[i++]);
  }
  printf("\n%d\n",count);
  printf("buffer: %s\n", rl_line_buffer);
  printf("position: %d\n", rl_point);
  printf("buf len %d\n", rl_end);
  rl_on_new_line();
  rl_insert_text("puk");
}

