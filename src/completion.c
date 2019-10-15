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
enum staging protostage[] = {START_STAGE, PROTO_STAGE, BUILTIN_STAGE, STOP_STAGE};
enum staging facestage[] = {START_STAGE, FACE_STAGE, COMMAND_STAGE, BUILTIN_STAGE, STOP_STAGE};
enum staging optionstage[] = {START_STAGE, OPTION_STAGE, COMMAND_STAGE, BUILTIN_STAGE, STOP_STAGE};

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
//          printf("BS %s\n", rootvalues);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case PROTO_STAGE:
        if (rootvalues = protovalues(text,len))
        {
//          printf("PS %s\n", rootvalues);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case FACE_STAGE:
        if (rootvalues = facevalues(text,len))
        {
//          printf("FS %s\n", rootvalues);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case OPTION_STAGE:
        if (rootvalues = optionvalues(text,len))
        {
//          printf("OS %s\n", rootvalues);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case COMMAND_STAGE:
        if (rootvalues = commandvalues(text,len))
        {
//          printf("CS %s\n", rootvalues);
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
//          printf("PS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case FACE_STAGE:
        if (subvalues = facevalues(text,len))
        {
//          printf("FS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case OPTION_STAGE:
        if (subvalues = optionvalues(text,len))
        {
//          printf("OS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case COMMAND_STAGE:
        if (subvalues = commandvalues(text,len))
        {
//          printf("CS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case SETTING_STAGE:
        if (subvalues = settingvalues(text,len, state))
        {
//          printf("SS %s\n", subvalues);
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case CUESETTING_STAGE:
        if (subvalues = cuesettingvalues(text,len, state))
        {
//          printf("CSS %s\n", subvalues);
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
//  printf("CNG %s %d\n", text,state);
  static int list_index, len;

  if (!state) {
      list_index = 0;
      len = strlen(text);
  }

  if (rl_commandname == NULL)
  {
//    puts("return rRC");
    return rl_rootcommands(text,len);
  }
  else
  {
//    puts("return rSC");
    return rl_subcommands(text,len, state);
  }

  return NULL;
}

static int compute_lcd_of_matches (char **match_list, int matches, const char *text)
{
  register int i, c1, c2, si;
  int low;		/* Count of max-matched characters. */
  int lx;
  char *dtext;		/* dequoted TEXT, if needed */

  /* If only one match, just use that.  Otherwise, compare each
     member of the list with the next, finding out where they
     stop matching. */
  if (matches == 1)
    {
      match_list[0] = match_list[1];
      match_list[1] = (char *)NULL;
      return 1;
    }

  for (i = 1, low = 100000; i < matches; i++)
    {
	  for (si = 0;
	       (c1 = match_list[i][si]) &&
	       (c2 = match_list[i + 1][si]);
	       si++)
	    if (c1 != c2)
	      break;

      if (low > si)
	low = si;
    }

  /* If there were multiple matches, but none matched up to even the
     first character, and the user typed something, use that as the
     value of matches[0]. */
  if (low == 0 && text && *text)
    {
      match_list[0] = (char *)malloc (strlen (text) + 1);
      strcpy (match_list[0], text);
    }
  else
    {
      match_list[0] = (char *)malloc (low + 1);
      strncpy (match_list[0], match_list[1], low);
      match_list[0][low] = '\0';
    }

  return matches;
}

char ** zc_completion_matches (const char *text, rl_compentry_func_t *entry_function)
{
  register int i;

  /* Number of slots in match_list. */
  int match_list_size;

  /* The list of matches. */
  char **match_list;

  /* Number of matches actually found. */
  int matches;

  /* Temporary string binder. */
  char *string;

  matches = 0;
  match_list_size = 10;
  match_list = (char **)malloc ((match_list_size + 1) * sizeof (char *));
  match_list[1] = (char *)NULL;

  while (string = (*entry_function) (text, matches))
    {

      if (matches + 1 >= match_list_size)
	match_list = (char **)realloc
	  (match_list, ((match_list_size += 10) + 1) * sizeof (char *));

      if (match_list == 0)
	return (match_list);

      match_list[++matches] = string;
      match_list[matches + 1] = (char *)NULL;
    }

  /* If there were any matches, then look through them finding out the
     lowest common denominator.  That then becomes match_list[0]. */
  if (matches)
    compute_lcd_of_matches (match_list, matches, text);
  else				/* There were no matches. */
    {
      free (match_list);
      match_list = (char **)NULL;
    }
  return (match_list);
}

int zc_completion(int count, int key)
{
  int i = 0;
  char **something;
  init_completition();

  //printf("buffer: |%s|\n", rl_line_buffer);
  //printf("position: %d\n", rl_point);
  //printf("buf len %d\n", rl_end);
  //rl_interpret(strdup(rl_line_buffer),1,0);
  char *rl_tokarr[100];
  int one = parse(strdup(rl_line_buffer), rl_tokarr);
  int numberoftokens = arrlength(rl_tokarr);
  //printf("one %d\n", one);
  //printf("number of tokens %d\n", numberoftokens);
  //printf("what |%c|\n", rl_line_buffer[rl_point-1]);

  if (rl_line_buffer[rl_point-1] == ' ' || (numberoftokens > 1))
  {
//    puts("\nsubcommand\n");
    rl_interpret(strdup(rl_line_buffer),1,rl_end);
  }
  else
    rl_interpret(strdup(rl_line_buffer),0,rl_end);

  if (one)
  {
    if (rl_line_buffer[rl_point-1] == ' ')
    {
      something = zc_completion_matches("", character_name_generator);
      if (something)
      {
        while (something[i])
        {
          i++;
          //puts(something[i++]);
        }
        //printf("1matches count %d\n",i);
        if (i == 1)
        {
          //printf("1replacing string with %s\n", something[i-1]);
          //putchar('\n');
          rl_insert_text(something[i-1]);
            rl_insert_text(" ");
        }
        else
        {
          i = 1;
          putchar('\n');
          while (something[i])
            puts(something[i++]);
          rl_on_new_line();
        }
      }
    }
    else
    {
      //printf("complete seq %s\n", rl_tokarr[one-1]);
      something = zc_completion_matches(rl_tokarr[one-1], character_name_generator);
      if (something)
      {
        while (something[i])
        {
          i++;
          //puts(something[i++]);
        }
        //printf("2matches count %d\n",i);
        if (i > 0)
        {
          //printf("2replacing string with %s\n", something[0]);
          //putchar('\n');
          rl_insert_text(&something[0][strlen(rl_tokarr[one-1])]);
          if (i == 1)
          {
            rl_insert_text(" ");
          }
          else
          {
            i = 1;
            putchar('\n');
            while (something[i])
              puts(something[i++]);
            rl_on_new_line();
          }
        }
      }
      else
      {
        //magic
        rl_insert_text(" ");
      }
    }
  }
  else
  {
    something = zc_completion_matches("", character_name_generator);
    if (something)
    {
      while (something[i])
      {
        i++;
        //puts(something[i++]);
      }
      if (i > 0)
      {
        if (i == 1)
        {
        }
        else
        {
          i = 1;
          putchar('\n');
          while (something[i])
            puts(something[i++]);
          rl_on_new_line();
        }
      }
      //printf("3matches count %d\n",i);
    }
  }
  //printf("count %d\n",count);
  //printf("buffer: |%s|\n", rl_line_buffer);
  //printf("position: %d\n", rl_point);
  //printf("buf len %d\n", rl_end);
  //rl_on_new_line();
  //rl_forced_update_display();
  //rl_insert_text("puk");
}
