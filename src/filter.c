#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement filter(WJElement input, WJElement schema, char * schemapath)
{
  WJElement output = WJEArray(NULL, NULL, WJE_NEW);
  WJElement ifaceinput = NULL, ifaceoutput = NULL;
  WJElement property = NULL;

  while (ifaceinput = _WJEObject(input,"values[]", WJE_GET, &ifaceinput))
  {
    ifaceoutput = WJEObject(output, "interface", WJE_NEW);
    while (property = WJEObjectF(schema, WJE_GET, &property, "%s.items.properties[]", schemapath))
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
        WJEAttach(ifaceoutput,WJEArray(ifaceinput, property->name, WJE_GET));
      }
    }
    WJEString(ifaceoutput,"name",WJE_NEW,ifaceinput->name);
  }

  return output;
}
