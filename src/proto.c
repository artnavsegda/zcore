#include <stdio.h>
#include "zcore.h"

char protopath[100] = "";
int protodepth = 0;

int listprotos(void)
{
  WJElement proto = NULL;
  puts("Protos:");
  while ((proto = WJEObjectF(root, WJE_GET, &proto, "%s[]", protopath))) {
    puts(proto->name);
  }
}

