#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <json-c/json.h>
//#include "interpreter.h"
//#include "completion.h"
//#include "config.h"
//#include "load.h"
//#include "acquire.h"
//#include "prompt.h"

json_object * root = NULL;
char zcore_prompt[255];

int main(int argc, char *argv[])
{
//  rl_attempted_completion_function = character_name_completion;

  //root = WJEObject(NULL, NULL, WJE_NEW);

//  readconfig();
//  loadeveryschema(root,config.schemapath);

//  acquireall(root);

//  WJEDump(root);

//  while (1)
//  {
//    generateprompt(zcore_prompt);
//    char * input = readline(zcore_prompt);
//    if (!input)
//      break;
//    add_history(input);
//    interpret(input);
//    free(input);
//  }

  return 0;
}
