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
extern enum domains domain;
char *rl_commandname = NULL;

int compute_lcd_of_matches2 (cmplist_t * list, char *text)
{
  register int i, c1, c2, si;
  int low;		/* Count of max-matched characters. */
  int lx;

  if (list->complecount == 1)
    {
      list->locode = list->complelist[0]->command;
      return 1;
    }

  for (i = 0, low = 100000; i < list->complecount-1; i++)
    {
	  for (si = 0;
	       (c1 = list->complelist[i]->command[si]) &&
	       (c2 = list->complelist[i + 1]->command[si]);
	       si++)
	    if (c1 != c2)
	      break;

      if (low > si)
	low = si;
    }

  if (low == 0 && text && *text)
    {
      list->locode = (char *)malloc (strlen (text) + 1);
      strcpy (list->locode, text);
    }
  else
    {
      list->locode = (char *)malloc (low + 1);
      strncpy (list->locode, list->complelist[0]->command, low);
      list->locode[low] = '\0';
    }

  return 0;
}

int sort_wrapper(const void *p1, const void *p2)
{
  return strcmp((*(cmpstr_t **)p1)->command,(*(cmpstr_t **)p2)->command);
}

void array_allocate(char * inputstring, callback_func_t *cb_func, cmplist_t * list)
{
  cmpstr_t *element;

  while (element = (*cb_func)(inputstring))
  {
    list->complecount++;
    list->complelist = (cmpstr_t **)realloc(list->complelist, sizeof(cmpstr_t *) * list->complecount);
    list->complelist[list->complecount-1] = element;
  }

  if (list->complecount)
  {
    qsort(list->complelist, list->complecount, sizeof (cmpstr_t *), sort_wrapper);
    compute_lcd_of_matches2(list, inputstring);
  }

}

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
  return 0;
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
  return 0;
}

void init_completition(void)
{
  incom_proto();
}

enum staging {START_STAGE, BUILTIN_STAGE, PROTO_STAGE, FACE_STAGE, COMMAND_STAGE, OPTION_STAGE, SETTING_STAGE, CUESETTING_STAGE, STOP_STAGE};
enum staging emptystage[] = {START_STAGE, STOP_STAGE};
enum staging protostage[] = {START_STAGE, PROTO_STAGE, BUILTIN_STAGE, STOP_STAGE};
enum staging facestage[] = {START_STAGE, FACE_STAGE, COMMAND_STAGE, BUILTIN_STAGE, STOP_STAGE};
enum staging optionstage[] = {START_STAGE, OPTION_STAGE, COMMAND_STAGE, BUILTIN_STAGE, STOP_STAGE};

cmpstr_t * rl_rootcommands2(const char * text, int len)
{
  static enum staging * cyclestaging = &emptystage[0];
  cmpstr_t * rootvalues = (cmpstr_t *)malloc(sizeof(cmpstr_t));

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
        if (rootvalues->command = builtinvalues(text,len))
        {
//          printf("BS %s\n", rootvalues);
          rootvalues->domain = BUILTIN;
          rootvalues->description = NULL;
          rootvalues->value = NULL;
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case PROTO_STAGE:
        if (rootvalues->command = protovalues(text,len))
        {
//          printf("PS %s\n", rootvalues);
          rootvalues->domain = PROTO;
//          rootvalues->description = protohelp(rootvalues->command);
          rootvalues->description = NULL;
          rootvalues->value = NULL;
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case FACE_STAGE:
        if (rootvalues->command = facevalues(text,len))
        {
//          printf("FS %s\n", rootvalues);
          rootvalues->domain = FACE;
          rootvalues->description = NULL;
          rootvalues->value = NULL;
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case OPTION_STAGE:
        if (rootvalues->command = optionvalues(text,len))
        {
//          printf("OS %s\n", rootvalues);
          rootvalues->description = optionhelp(rootvalues->command);
          rootvalues->domain = OPTION;
          rootvalues->value = optionvalue(rootvalues->command);
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case COMMAND_STAGE:
        if (rootvalues->command = commandvalues(text,len))
        {
//          printf("CS %s\n", rootvalues);
          rootvalues->domain = COMMAND;
          rootvalues->description = NULL;
          rootvalues->value = NULL;
          return rootvalues;
        }
        else
          cyclestaging++;
      break;
      case STOP_STAGE:
        cyclestaging = &emptystage[0];
        free(rootvalues);
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

cmpstr_t * rl_subcommands2(const char * text, int len, int state)
{
  static enum staging * cyclestaging = &emptystage[0];
  cmpstr_t * subvalues = (cmpstr_t *)malloc(sizeof(cmpstr_t));

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
        if (subvalues->command = protovalues(text,len))
        {
//          printf("PS %s\n", subvalues);
          subvalues->domain = PROTO;
          subvalues->description = NULL;
          subvalues->value = NULL;
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case FACE_STAGE:
        if (subvalues->command = facevalues(text,len))
        {
//          printf("FS %s\n", subvalues);
          subvalues->domain = FACE;
          subvalues->description = NULL;
          subvalues->value = NULL;
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case OPTION_STAGE:
        if (subvalues->command = optionvalues(text,len))
        {
//          printf("OS %s\n", subvalues);
          subvalues->domain = OPTION;
          subvalues->description = optionhelp(subvalues->command);
          subvalues->value = NULL;
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case COMMAND_STAGE:
        if (subvalues->command = commandvalues(text,len))
        {
//          printf("CS %s\n", subvalues);
          subvalues->domain = COMMAND;
          subvalues->description = NULL;
          subvalues->value = NULL;
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case SETTING_STAGE:
        if (subvalues->command = settingvalues(text,len, state))
        {
//          printf("SS %s\n", subvalues);
          subvalues->domain = SETTING;
          subvalues->description = NULL;
          subvalues->value = NULL;
          return subvalues;
        }
        else
          cyclestaging++;
      break;
      case CUESETTING_STAGE:
        if (subvalues->command = cuesettingvalues(text,len, state))
        {
//          printf("CSS %s\n", subvalues);
          subvalues->domain = CUESETTING;
          subvalues->description = NULL;
          subvalues->value = NULL;
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

cmpstr_t *callback(char * inputstring)
{
  if (rl_commandname == NULL)
    return rl_rootcommands2(inputstring, strlen(inputstring));
  else
    return rl_subcommands2(inputstring, strlen(inputstring), 0);
}

void print_cmp_list(cmplist_t *list)
{
  int x = 1;
  for (int i = PROTO; i <= BUILTIN; i++)
  {
    if (x == 1)
    {
      putchar('\n');
      x = 0;
    }
    for (int y = 0; y < list->complecount; y++)
    {
      if(list->complelist[y]->domain == i)
      {
        printf("\t%s", list->complelist[y]->command);
        if(list->complelist[y]->description)
        {
          printf(":\t%s", list->complelist[y]->description);
        }
        if(list->complelist[y]->value)
        {
          printf(":\t%s", list->complelist[y]->value);
        }
        puts("");
        x = 1;
      }
    }
  }
  rl_on_new_line();

  return;
}

void zc_cleanup(cmplist_t *list)
{
  if (list->locode)
  {
    free(list->locode);
  }
  if (list->complecount > 1)
  {
    for (int i = 0; i < list->complecount; i++)
    {
      free(list->complelist[i]->command);
      if (list->complelist[i]->value)
        free(list->complelist[i]->value);
      free(list->complelist[i]);
    }
  }
}

int zc_completion2(int count, int key)
{
  cmplist_t list = { .complecount = 0 };
  int i = 0;
  char **something;
  init_completition();
  char *rl_tokarr[100];
  int one = parse(strdup(rl_line_buffer), rl_tokarr);
  int numberoftokens = arrlength(rl_tokarr);

  if (rl_line_buffer[rl_point-1] == ' ' || (numberoftokens > 1))
  {
    rl_interpret(strdup(rl_line_buffer),1,rl_end);
  }
  else
    rl_interpret(strdup(rl_line_buffer),0,rl_end);

  if (one)
  {
    if (rl_line_buffer[rl_point-1] == ' ')
    {
      array_allocate("", callback, &list);
      if (list.complecount)
      {
        if (list.complecount == 1)
        {
          rl_insert_text(list.locode);
            rl_insert_text(" ");
        }
        else
        {
          print_cmp_list(&list);
        }
      }
    }
    else
    {
      array_allocate(rl_tokarr[one-1], callback, &list);
      if (list.complecount)
      {
          rl_insert_text(&(list.locode[strlen(rl_tokarr[one-1])]));
          if (list.complecount == 1)
          {
            rl_insert_text(" ");
          }
          else
          {
            print_cmp_list(&list);
          }
      }
      else
      {
        rl_insert_text(" ");
      }
    }
  }
  else
  {
    array_allocate("", callback, &list);
    if (list.complecount)
    {
      if (list.complecount > 0)
      {
        if (list.complecount == 1)
        {
        }
        else
        {
          print_cmp_list(&list);
        }
      }
    }
  }
  zc_cleanup(&list);

  return 0;
}
