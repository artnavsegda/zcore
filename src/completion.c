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

int _rl_completion_case_fold = 0;
#  define PTR_T void *
typedef int QSFUNC (const void *, const void *);

#define RL_CHECK_SIGNALS() \
        do { \
          if (_rl_caught_signal) _rl_signal_handler (_rl_caught_signal); \
        } while (0)

#define RL_SIG_RECEIVED() (_rl_caught_signal != 0)

extern int volatile _rl_caught_signal;

extern void _rl_signal_handler PARAMS((int));

#  define FREE(x)    if (x) free (x)

/* Stupid comparison routine for qsort () ing strings. */
int
_rl_qsort_string_compare (s1, s2)
  char **s1, **s2;
{
#if defined (HAVE_STRCOLL)
  return (strcoll (*s1, *s2));
#else
  int result;

  result = **s1 - **s2;
  if (result == 0)
    result = strcmp (*s1, *s2);

  return result;
#endif
}

static int
compute_lcd_of_matches (match_list, matches, text)
     char **match_list;
     int matches;
     const char *text;
{
  register int i, c1, c2, si;
  int low;		/* Count of max-matched characters. */
  int lx;
  char *dtext;		/* dequoted TEXT, if needed */
#if defined (HANDLE_MULTIBYTE)
  int v;
  size_t v1, v2;
  mbstate_t ps1, ps2;
  wchar_t wc1, wc2;
#endif

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
#if defined (HANDLE_MULTIBYTE)
      if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	{
	  memset (&ps1, 0, sizeof (mbstate_t));
	  memset (&ps2, 0, sizeof (mbstate_t));
	}
#endif
      if (_rl_completion_case_fold)
	{
	  for (si = 0;
	       (c1 = _rl_to_lower(match_list[i][si])) &&
	       (c2 = _rl_to_lower(match_list[i + 1][si]));
	       si++)
#if defined (HANDLE_MULTIBYTE)
	    if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	      {
		v1 = mbrtowc(&wc1, match_list[i]+si, strlen (match_list[i]+si), &ps1);
		v2 = mbrtowc (&wc2, match_list[i+1]+si, strlen (match_list[i+1]+si), &ps2);
		if (MB_INVALIDCH (v1) || MB_INVALIDCH (v2))
		  {
		    if (c1 != c2)	/* do byte comparison */
		      break;
		    continue;
		  }
		wc1 = towlower (wc1);
		wc2 = towlower (wc2);
		if (wc1 != wc2)
		  break;
		else if (v1 > 1)
		  si += v1 - 1;
	      }
	    else
#endif
	    if (c1 != c2)
	      break;
	}
      else
	{
	  for (si = 0;
	       (c1 = match_list[i][si]) &&
	       (c2 = match_list[i + 1][si]);
	       si++)
#if defined (HANDLE_MULTIBYTE)
	    if (MB_CUR_MAX > 1 && rl_byte_oriented == 0)
	      {
		mbstate_t ps_back;
		ps_back = ps1;
		if (!_rl_compare_chars (match_list[i], si, &ps1, match_list[i+1], si, &ps2))
		  break;
		else if ((v = _rl_get_char_len (&match_list[i][si], &ps_back)) > 1)
		  si += v - 1;
	      }
	    else
#endif
	    if (c1 != c2)
	      break;
	}

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

      /* XXX - this might need changes in the presence of multibyte chars */

      /* If we are ignoring case, try to preserve the case of the string
	 the user typed in the face of multiple matches differing in case. */
      if (_rl_completion_case_fold)
	{
	  /* We're making an assumption here:
		IF we're completing filenames AND
		   the application has defined a filename dequoting function AND
		   we found a quote character AND
		   the application has requested filename quoting
		THEN
		   we assume that TEXT was dequoted before checking against
		   the file system and needs to be dequoted here before we
		   check against the list of matches
		FI */
	  dtext = (char *)NULL;
	  if (rl_filename_completion_desired &&
	      rl_filename_dequoting_function &&
	      rl_completion_found_quote &&
	      rl_filename_quoting_desired)
	    {
	      dtext = (*rl_filename_dequoting_function) ((char *)text, rl_completion_quote_character);
	      text = dtext;
	    }

	  /* sort the list to get consistent answers. */
	  qsort (match_list+1, matches, sizeof(char *), (QSFUNC *)_rl_qsort_string_compare);

	  si = strlen (text);
	  lx = (si <= low) ? si : low;	/* check shorter of text and matches */
	  /* Try to preserve the case of what the user typed in the presence of
	     multiple matches: check each match for something that matches
	     what the user typed taking case into account; use it up to common
	     length of matches if one is found.  If not, just use first match. */
	  for (i = 1; i <= matches; i++)
	    if (strncmp (match_list[i], text, lx) == 0)
	      {
		strncpy (match_list[0], match_list[i], low);
		break;
	      }
	  /* no casematch, use first entry */
	  if (i > matches)
	    strncpy (match_list[0], match_list[1], low);

	  FREE (dtext);
	}
      else
        strncpy (match_list[0], match_list[1], low);

      match_list[0][low] = '\0';
    }

  return matches;
}

char **
zc_completion_matches (text, entry_function)
     const char *text;
     rl_compentry_func_t *entry_function;
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
      if (RL_SIG_RECEIVED ())
	{
	  /* Start at 1 because we don't set matches[0] in this function.
	     Only free the list members if we're building match list from
	     rl_filename_completion_function, since we know that doesn't
	     free the strings it returns. */
	  if (entry_function == rl_filename_completion_function)
	    {
	      for (i = 1; match_list[i]; i++)
		free (match_list[i]);
	    }
	  free (match_list);
	  match_list = 0;
	  match_list_size = 0;
	  matches = 0;
	  RL_CHECK_SIGNALS ();
	}

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
