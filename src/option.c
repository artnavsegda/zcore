#include <stdio.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"
#include "option.h"

extern WJElement protojson;

int listoptions(void)
{
  WJElement option = NULL;
  puts("Options:");

  while ((option = _WJEObject(protojson, "schema.items.properties[]", WJE_GET, &option))) {
    puts(option->name);
  }
}

int isoption(char * optionname)
{
  if (domain == OPTION)
  {
    if (WJEGetF(protojson, NULL, "schema.items.properties.%s", optionname))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

