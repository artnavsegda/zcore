#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement filter(WJReader inputstream, WJElement schema, char * schemapath)
{
  WJElement input = WJEOpenDocument(inputstream, NULL, NULL, NULL);
  WJElement output = WJEArray(NULL, NULL, WJE_NEW);
  WJElement ifaceinput = NULL, ifaceoutput = NULL;
  WJElement property = NULL;

  while (ifaceinput = _WJEObject(input,"values[]", WJE_GET, &ifaceinput))
  {
    ifaceoutput = WJEObject(output, "interface", WJE_NEW);
    WJEString(ifaceoutput,"name",WJE_NEW,ifaceinput->name);

    while (property = _WJEObject(schema,schemapath, WJE_GET, &property))
    {
      if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"string") == 0)
      {
        char * stringvalue = WJEString(ifaceinput,property->name,WJE_GET,"");
        if (stringvalue[0])
        {
          WJEString(ifaceoutput,property->name,WJE_NEW,stringvalue);
        }
      }
      else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"number") == 0)
      {
        int numbervalue = WJEInt32(ifaceinput,property->name,WJE_GET,-1);
        if (numbervalue != -1)
        {
          WJEInt32(ifaceoutput,property->name,WJE_NEW,numbervalue);
        }
      }
      else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"boolean") == 0)
      {
        int boolval = WJEInt32(ifaceinput,property->name,WJE_GET,-1);
        if (boolval != -1)
        {
          WJEBool(ifaceoutput,property->name,WJE_NEW,(XplBool)boolval);
        }
      }
      else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"array") == 0)
      {
        //WJEDump(WJEArray(ifaceinput, property->name, WJE_GET));
        //WJEMergeObjects(WJEArray(ifaceoutput, property->name, WJE_NEW),WJEArray(ifaceinput, property->name, WJE_GET), TRUE);
        //WJEMergeObjects(ifaceoutput,WJEArray(ifaceinput, property->name, WJE_GET), TRUE);
        WJEAttach(ifaceoutput,WJEArray(ifaceinput, property->name, WJE_GET));
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
  }

  return output;
}
