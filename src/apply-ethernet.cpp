#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement filter(WJReader inputstream)
{
  WJElement input = WJEOpenDocument(inputstream, NULL, NULL, NULL);
  WJElement output = WJEObject(NULL, NULL, WJE_NEW);
  WJElement valuesoutput = NULL;

  WJEString(output,"config",WJE_NEW,"network");

  char * name = WJEString(input,"name",WJE_GET,"");
  if (name[0]){
    WJEString(output,"section",WJE_NEW,name);
  }

  valuesoutput = WJEObject(output, "values", WJE_NEW);

  char * macaddr = WJEString(input,"macaddr",WJE_GET,"");
  if (macaddr[0]){
    WJEString(valuesoutput,"macaddr",WJE_NEW,macaddr);
  }

  XplBool defaultroute = WJEBool(input,"defaultroute",WJE_GET,-1);
  if (defaultroute != -1){
    WJEInt32(valuesoutput,"defaultroute",WJE_NEW,(int)defaultroute);
  }

  int metric = WJEInt32(input,"metric",WJE_GET,-1);
  if (metric != -1){
    WJEInt32(valuesoutput,"metric",WJE_NEW,(int)metric);
  }

  return output;
}
