#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include <unistd.h>
#include <limits.h>
#include "interpreter.h"
#include "completion.h"
#include "config.h"
#include "load.h"
#include "acquire.h"

WJElement root = NULL;

char zcore_prompt[255] = "";

void generateprompt(char * prompt)
{
  char hostname[HOST_NAME_MAX];
  gethostname(hostname, HOST_NAME_MAX);
  sprintf(prompt, "%s>", hostname);
}

int main(int argc, char *argv[])
{
  rl_attempted_completion_function = character_name_completion;

  root = WJEObject(NULL, NULL, WJE_NEW);

  readconfig();
  loadeveryscheme(root,config.schemepath);

  acquireall(root);

  //WJEDump(root);

  while (1)
  {
    generateprompt(zcore_prompt);
    char * input = readline(zcore_prompt);
    if (!input)
      break;
    add_history(input);
    interpret(input);
    free(input);
  }

  return 0;
}
