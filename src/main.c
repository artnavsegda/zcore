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

void ctrl_c(int signal) {
   printf ("ctrl-c key pressed\n");
   rl_on_new_line ();
}

int my_cool_readline_func (int count, int key) {
   printf ("key pressed: %d\n", key);
   rl_on_new_line ();
   return 0;
}

void test_cmp(int signal) {
  zc_completion2(0, 0);
    rl_forced_update_display();
}

int main(int argc, char *argv[])
{
  signal(SIGALRM, alarm_handler);
  signal(SIGINT, ctrl_c);
  signal(SIGUSR1, test_cmp);
//  rl_attempted_completion_function = character_name_completion;
  rl_bind_key('\t', zc_completion2);
  rl_bind_keyseq("\\C-c", my_cool_readline_func);

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
