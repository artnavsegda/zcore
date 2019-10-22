#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "utils.h"
#include "interpreter.h"
#include "proto.h"
#include "builtin.h"
#include "face.h"
// #include "command.h"
#include "option.h"

enum domains domain = PROTO;
extern json_object * protojson;
extern json_object * protoface;
extern int protodepth;

int execute(int argc, char *argv[]) {
  int ret = 0;
  enum domains ret_domain = domain;
  json_object * ret_protojson = protojson;
  json_object * ret_protoface = protoface;
  int ret_protodepth = protodepth;

  if (isbuiltin(argv[0]))
  {
    ret = builtin(argc,argv);
  }
  else if (isproto(argv[0]))
  {
    ret = proto(argc,argv);
  }
  // else if (iscommand(argv[0]))
  // {
  //   ret = command(argc,argv);
  // }
  else if (isface(argv[0]))
  {
    ret = face(argc,argv);
  }
  else if (isoption(argv[0]))
  {
    ret = option(argc,argv);
  }
  else
  {
    puts("invalid input");
  }
  if (ret)
  {
    domain = ret_domain;
    protojson = ret_protojson;
    protoface = ret_protoface;
    protodepth = ret_protodepth;
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
