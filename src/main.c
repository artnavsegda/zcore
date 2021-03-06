#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include <unistd.h>
#include <errno.h>
#include "interpreter.h"
#include "completion.h"
#include "config.h"
#include "load.h"
#include "acquire.h"
#include "prompt.h"

#define MAXLEN 1000

WJElement root = NULL;
char zcore_prompt[255];

int debug = 0;

void ctrl_c(int signal) {
   //printf ("ctrl-c key pressed\n");
   rl_replace_line("", 1);
   puts("");
   //rl_line_buffer[0] = '\0';
   rl_forced_update_display();
}

void handle_sigchld(int sig) {
  int saved_errno = errno;
  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {}
  errno = saved_errno;
}

int main(int argc, char *argv[])
{
  int opt;
  char *zcoreconfig = NULL;

  while ((opt = getopt(argc, argv, "dc:")) != -1)
  {
    switch (opt)
    {
      case 'd': // debug
        debug = 1;
        break;
      case 'c': // config
        zcoreconfig = optarg;
        break;
    }
  }

  root = WJEObject(NULL, NULL, WJE_NEW);

  readconfig(zcoreconfig);
  loadeveryschema(root,config.schemapath);
  //WJEDump(root);
  acquireall(root);

  if (argv[optind])
  {
    //script mode
    //puts("script mode");
    FILE *script = fopen(argv[optind],"r");
    if (script == NULL)
    {
      printf("cannot open script file %s\n",argv[optind]);
      return 1;
    }

    char line[MAXLEN];
    while (fgets(line, MAXLEN, script))
    {
      if (line[0] != '#')
        interpret(line);
      //printf("line: %s", line);
    }

    return 0;
  }

  signal(SIGINT, ctrl_c);
//  signal(SIGCHLD, SIG_IGN);
//  signal(SIGCHLD, handle_sigchld);

//  rl_attempted_completion_function = character_name_completion;
  rl_bind_key('\t', zc_completion2);
//  rl_bind_key('>', zc_completion3);
  rl_bind_key('?', zc_completion3);

  //WJEDump(root);

  while (1)
  {
    generateprompt(zcore_prompt);
    char * input = readline(zcore_prompt);
    if (!input)
      break;
    char * nwinput = input;

    while(*nwinput == ' ' || *nwinput == '\t')
      nwinput++;

    if (strlen(nwinput) > 0)
    {
      add_history(nwinput);
      interpret(nwinput);
    }
    free(input);
  }

  return 0;
}
