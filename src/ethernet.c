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

    char * macaddr = WJEString(ifaceinput,"macaddr",WJE_GET,"");
    if (macaddr[0]){
      WJEString(ifaceoutput,"macaddr",WJE_NEW,macaddr);
    }

    int defaultroute = WJEInt32(ifaceinput,"defaultroute",WJE_GET,-1);
    if (defaultroute != -1){
      WJEBool(ifaceoutput,"defaultroute",WJE_NEW,(XplBool)defaultroute);
    }

    int metric = WJEInt32(ifaceinput,"metric",WJE_GET,-1);
    if (metric != -1) {
      WJEInt32(ifaceoutput,"metric",WJE_NEW,metric);
    }
  }

  return output;
}
