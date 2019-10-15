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

enum staging {START_STAGE, BUILTIN_STAGE, PROTO_STAGE, FACE_STAGE, COMMAND_STAGE, OPTION_STAGE, SETTING_STAGE, CUESETTING_STAGE, STOP_STAGE};
enum staging emptystage[] = {START_STAGE, STOP_STAGE};
enum staging protostage[] = {START_STAGE, BUILTIN_STAGE, PROTO_STAGE, STOP_STAGE};
enum staging facestage[] = {START_STAGE, BUILTIN_STAGE, FACE_STAGE, COMMAND_STAGE, STOP_STAGE};
enum staging optionstage[] = {START_STAGE, BUILTIN_STAGE, OPTION_STAGE, COMMAND_STAGE, STOP_STAGE};

char * rl_rootcommands(const char * text, int len)
{
  static enum staging * cyclestaging = &emptystage[0];
  char * rootvalues = NULL;

  while (1)
  {
    switch (*cyclestaging)
    {
      case START_STAGE:
        switch (domain)
        {
          case PROTO:
            cyclestaging = &protostage[0];
          break;
          case FACE:
            cyclestaging = &facestage[0];
          break;
          case OPTION:
            cyclestaging = &optionstage[0];
          break;
        }
        cyclestaging++;
      break;
      case BUILTIN_STAGE:
        if (rootvalues = builtinvalues(text,len))
        {
          printf("BS %s\n", rootvalues);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case PROTO_STAGE:
        if (rootvalues = protovalues(text,len))
        {
          printf("PS %s\n", rootvalues);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case FACE_STAGE:
        if (rootvalues = facevalues(text,len))
        {
          printf("FS %s\n", rootvalues);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case OPTION_STAGE:
        if (rootvalues = optionvalues(text,len))
        {
          printf("OS %s\n", rootvalues);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case COMMAND_STAGE:
        if (rootvalues = commandvalues(text,len))
        {
          printf("CS %s\n", rootvalues);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case STOP_STAGE:
        cyclestaging = &emptystage[0];
        return NULL;
      break;
    }
  }
}

enum staging sub_protostage[] = {START_STAGE, PROTO_STAGE, STOP_STAGE};
enum staging sub_facestage[] = {START_STAGE, FACE_STAGE, STOP_STAGE};
enum staging sub_optionstage[] = {START_STAGE, OPTION_STAGE, COMMAND_STAGE, STOP_STAGE};
enum staging sub_settingstage[] = {START_STAGE, SETTING_STAGE, CUESETTING_STAGE, STOP_STAGE};
enum staging sub_commandstage[] = {START_STAGE, COMMAND_STAGE, STOP_STAGE};

char * rl_subcommands(const char * text, int len, int state)
{
  static enum staging * cyclestaging = &emptystage[0];
  char * subvalues = NULL;

  while (1)
  {
    switch (*cyclestaging)
    {
      case START_STAGE:
        switch (rl_domain)
        {
          case PROTO:
            cyclestaging = &sub_protostage[0];
          break;
          case FACE:
            cyclestaging = &sub_facestage[0];
          break;
          case OPTION:
            cyclestaging = &sub_optionstage[0];
          break;
          case SETTING:
            cyclestaging = &sub_settingstage[0];
          break;
          case COMMAND:
            cyclestaging = &sub_commandstage[0];
          break;
        }
        cyclestaging++;
      break;
      case PROTO_STAGE:
        if (subvalues = protovalues(text,len))
        {
          printf("PS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case FACE_STAGE:
        if (subvalues = facevalues(text,len))
        {
          printf("FS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case OPTION_STAGE:
        if (subvalues = optionvalues(text,len))
        {
          printf("OS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case COMMAND_STAGE:
        if (subvalues = commandvalues(text,len))
        {
          printf("CS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case SETTING_STAGE:
        if (subvalues = settingvalues(text,len, state))
        {
          printf("SS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case CUESETTING_STAGE:
        if (subvalues = cuesettingvalues(text,len, state))
        {
          printf("CSS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case STOP_STAGE:
        cyclestaging = &emptystage[0];
        return NULL;
      break;
    }
  }
}

char * character_name_generator(const char *text, int state)
{
  printf("CNG %s %d\n", text,state);
  static int list_index, len;

  if (!state) {
      list_index = 0;
      len = strlen(text);
  }

  if (rl_commandname == NULL)
  {
    puts("return rRC");
    return rl_rootcommands(text,len);
  }
  else
  {
    puts("return rSC");
    return rl_subcommands(text,len, state);
  }

  return NULL;
}

int zc_completion(int count, int key)
{
  int i = 0;
  char **something;
  init_completition();

  if (rl_line_buffer[rl_point-1] == ' ')
  {
    puts("\ninnouit\n");
    rl_interpret(strdup(rl_line_buffer),1,rl_end);
  }
  else
    rl_interpret(strdup(rl_line_buffer),0,rl_end);

  printf("buffer: |%s|\n", rl_line_buffer);
  printf("position: %d\n", rl_point);
  printf("buf len %d\n", rl_end);
  //rl_interpret(strdup(rl_line_buffer),1,0);
  char *rl_tokarr[100];
  int one = parse(strdup(rl_line_buffer), rl_tokarr);
  int numberoftokens = arrlength(rl_tokarr);
  printf("one %d\n", one);
  printf("number of tokens %d\n", numberoftokens);
  printf("what |%c|\n", rl_line_buffer[rl_point-1]);

  if (one)
  {
    if (rl_line_buffer[rl_point-1] == ' ')
    {
      something = rl_completion_matches("", character_name_generator);
      if (something)
      {
        while (something[i])
          puts(something[i++]);
      }
      printf("matches count %d\n",i);
      if (i == 1)
      {
        printf("replacing string with %s\n", something[i-1]);
        rl_insert_text(&something[i-1][rl_point]);
        rl_insert_text(" ");
      }
    }
    else
    {
      printf("complete seq %s\n", rl_tokarr[one-1]);
      something = rl_completion_matches(rl_tokarr[one-1], character_name_generator);
      if (something)
      {
        while (something[i])
          puts(something[i++]);
      }
      printf("matches count %d\n",i);
      if (i == 1)
      {
        printf("replacing string with %s\n", something[i-1]);
        rl_insert_text(&something[i-1][rl_point]);
        rl_insert_text(" ");
      }
    }
  }
  else
  {
    something = rl_completion_matches("", character_name_generator);
    if (something)
    {
      while (something[i])
        puts(something[i++]);
    }
    printf("matches count %d\n",i);
  }
  printf("count %d\n",count);
  printf("buffer: |%s|\n", rl_line_buffer);
  printf("position: %d\n", rl_point);
  printf("buf len %d\n", rl_end);
  rl_on_new_line();
  //rl_insert_text("puk");
}
