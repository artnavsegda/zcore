#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include <unistd.h>
#include "interpreter.h"
#include "completion.h"
#include "config.h"
#include "load.h"
#include "acquire.h"
#include "prompt.h"

WJElement root = NULL;
char zcore_prompt[255];

void alarm_handler(int signal)
{
  acquireall(root);
}

int main(int argc, char *argv[])
{
  signal(SIGALRM, alarm_handler);
  signal(SIGINT, SIG_IGN);
//  rl_attempted_completion_function = character_name_completion;
  rl_bind_key('\t', zc_completion2);

  root = WJEObject(NULL, NULL, WJE_NEW);

  readconfig();
  loadeveryschema(root,config.schemapath);

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
