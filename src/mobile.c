#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement filter(WJReader inputstream)
{
  WJElement input = WJEOpenDocument(inputstream, NULL, NULL, NULL);
  WJElement output = WJEArray(NULL, NULL, WJE_NEW);
  WJElement ifaceinput = NULL, ifaceoutput = NULL;

  while (ifaceinput = _WJEObject(input,"values[]", WJE_GET, &ifaceinput))
  {
    ifaceoutput = WJEObject(output, "interface", WJE_NEW);
    WJEString(ifaceoutput,"name",WJE_NEW,ifaceinput->name);

    char * apn = WJEString(ifaceinput,"apn",WJE_GET,"");
    if (apn[0]){
      WJEString(ifaceoutput,"apn",WJE_NEW,apn);
    }

    int enable = WJEInt32(ifaceinput,"enable",WJE_GET,-1);
    if (enable != -1){
      WJEBool(ifaceoutput,"enable",WJE_NEW,(XplBool)enable);
    }
  }

  return output;
}
