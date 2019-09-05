#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include "utils.h"
#include "interpreter.h"
#include "proto.h"
#include "builtin.h"
#include "face.h"
#include "command.h"
#include "option.h"

enum domains domain = PROTO;

int execute(int argc, char *argv[]) {
  if (isbuiltin(argv[0]))
  {
    builtin(argc,argv);
  }
  else if (isproto(argv[0]))
  {
    proto(argc,argv);
  }
  else if (iscommand(argv[0]))
  {
    command(argc,argv);
  }
  else if (isface(argv[0]))
  {
    face(argc,argv);
  }
  else if (isoption(argv[0]))
  {
    option(argc,argv);
  }
}

int interpret(char * stringtointerpret)
{
  char *tokarr[100]; // maximum argument count
  int one = parse(stringtointerpret, tokarr);
  int numberoftokens = arrlength(tokarr);
  if (numberoftokens > 0)
  {
    execute(numberoftokens,tokarr);
  }
}
