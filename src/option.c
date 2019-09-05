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

