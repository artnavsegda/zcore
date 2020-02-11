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
#include "domain.h"
#include "global.h"

enum domains domain = PROTO;
extern WJElement protojson;
extern WJElement protoschema;
extern WJElement protoface;
extern int protodepth;
extern int optiondepth;
extern enum domains domain;

int execute(int argc, char *argv[]) {
  int ret = 0;
  enum domains ret_domain = domain;
  WJElement ret_protojson = protojson;
  WJElement ret_protoschema = protoschema;
  WJElement ret_protoface = protoface;
  int ret_protodepth = protodepth;
  int ret_optiondepth = optiondepth;

  if (isbuiltin(argv[0]))
  {
    ret = builtin(argc,argv);
  }
  else if (isproto(argv[0]))
  {
    ret = proto(argc,argv);
  }
  else if (iscommand(argv[0]))
  {
    ret = command(argc,argv);
  }
  else if (isface(argv[0]))
  {
    ret = face(argc,argv);
  }
  else if (isoption(argv[0]))
  {
    ret = option(argc,argv);
  }
  else if (isglobal(argv[0]))
  {
    ret = global(argc,argv);
  }
  else
  {
    puts("invalid input");
  }
  if (ret)
  {
    domain = ret_domain;
    protojson = ret_protojson;
    protoschema = ret_protoschema;
//    protoface = ret_protoface;
    protodepth = ret_protodepth;
    optiondepth = ret_optiondepth;
  }
  return ret;
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
  return 0;
}
