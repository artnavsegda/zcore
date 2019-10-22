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

  char * apn = WJEString(input,"apn",WJE_GET,"");
  if (apn[0]){
    WJEString(valuesoutput,"apn",WJE_NEW,apn);
  }

  XplBool enable = WJEBool(input,"enable",WJE_GET,-1);
  if (enable != -1){
    WJEInt32(valuesoutput,"enable",WJE_NEW,(int)enable);
  }

  return output;
}
