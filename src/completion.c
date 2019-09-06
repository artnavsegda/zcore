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

char *command_rl;

char * find_command(char **tokarr, int start)
{
  int numberoftokens = arrlength(tokarr);
  if (numberoftokens > 0)
  {
    if (start > 0)
      return tokarr[0];
  }
  return NULL;
}

char ** character_name_completion(const char *text, int start, int end)
{
  char *tokarr[100];
  char *inputline = strdup(rl_line_buffer);
  parse(inputline, tokarr);
  command_rl = find_command(tokarr, start);
  rl_attempted_completion_over = 1;
  return rl_completion_matches(text, character_name_generator);
}

char * character_name_generator(const char *text, int state)
{
  static int list_index, len;

  if (!state) {
      list_index = 0;
      len = strlen(text);
  }

  //printf("DEBUG %s\n", command_rl);

  return protovalues(text,len);

  return NULL;
}
