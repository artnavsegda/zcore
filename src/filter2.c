#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement filter(WJReader inputstream, WJElement schema, char * schemapath)
{
  WJElement input = WJEOpenDocument(inputstream, NULL, NULL, NULL);
  WJElement output = WJEObject(NULL, NULL, WJE_NEW);
  WJElement property = NULL;
  WJElement valuesoutput = NULL;

  WJEString(output,"config",WJE_NEW,"network");

  char * name = WJEString(input,"name",WJE_GET,"");
  if (name[0]){
    WJEString(output,"section",WJE_NEW,name);
  }

  valuesoutput = WJEObject(output, "values", WJE_NEW);

    while (property = _WJEObject(schema,schemapath, WJE_GET, &property))
    {
      if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"string") == 0)
      {
        char * stringvalue = WJEString(input,property->name,WJE_GET,"");
        if (stringvalue[0])
        {
          WJEString(valuesoutput,property->name,WJE_NEW,stringvalue);
        }
      }
      else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"number") == 0)
      {
        int numbervalue = WJEInt32(input,property->name,WJE_GET,-1);
        if (numbervalue != -1)
        {
          WJEInt32(valuesoutput,property->name,WJE_NEW,numbervalue);
        }
      }
      else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"boolean") == 0)
      {
        XplBool boolval = WJEBool(input,property->name,WJE_GET,-1);
        if (boolval != -1)
        {
          WJEInt32(valuesoutput,property->name,WJE_NEW,(int)boolval);
        }
      }
    }

//    int defaultroute = WJEInt32(ifaceinput,"defaultroute",WJE_GET,-1);
//    if (defaultroute != -1){
//      WJEBool(ifaceoutput,"defaultroute",WJE_NEW,(XplBool)defaultroute);
//    }

//    int metric = WJEInt32(ifaceinput,"metric",WJE_GET,-1);
//    if (metric != -1) {
//      WJEInt32(ifaceoutput,"metric",WJE_NEW,metric);
//    }

  return output;
}
