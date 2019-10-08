#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

void translate(WJElement ifaceoutput, WJElement ifaceinput, WJElement schema, char * schemapath, char * protopath)
{
  WJElement property = NULL;
  while (property = WJEObjectF(schema, WJE_GET, &property, protopath, schemapath))
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
}

WJElement filter(WJElement input, WJElement schema, char * schemapath)
{
  WJElement ifaceinput = NULL, ifaceoutput = NULL;

  if (WJEGet(schema,"patternProperties",NULL))
  {
    WJElement output = WJEObject(NULL, NULL, WJE_NEW);
    while (ifaceinput = _WJEObject(input,"[]", WJE_GET, &ifaceinput))
    {
      ifaceoutput = WJEObject(output, "interface", WJE_NEW);
      translate(ifaceoutput, ifaceinput, schema, schemapath, "%s.items.properties[]");
      if (WJEBool(ifaceinput, "[\".anonymous\"]", WJE_GET, FALSE))
      {
        WJEString(ifaceoutput,"section",WJE_NEW,ifaceinput->name);
      }
      if (!WJEGet(ifaceoutput, "name", NULL))
        WJEString(ifaceoutput,"name",WJE_NEW,ifaceinput->name);
      WJERename(ifaceoutput, WJEGet(ifaceoutput, "name", NULL));
    }
    return output;
  }
  else if (WJEGet(schema,"properties",NULL))
  {
    WJElement output = WJEObject(NULL, NULL, WJE_NEW);
    translate(output, WJEObject(input, "values", WJE_GET), schema, schemapath, "%s.properties[]");
    return output;
  }
}
