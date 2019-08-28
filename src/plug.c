#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include "filter.h"

WJElement doc = NULL;

int main(int argc, char *argv[])
{
  FILE *jsonstream;
  WJReader readjson;
  char temp[100];
  if (!(jsonstream = popen(argv[1], "r"))) {
    puts("handle error");
    return 1;
  }

  if (!(readjson = WJROpenFILEDocument(jsonstream, NULL, 0))) {
    puts("json failed to open");
    return 1;
  }

  doc = filter(readjson);
  WJEDump(doc);

  return 0;
}
