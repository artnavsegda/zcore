#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <wjelement.h>
#include <wjreader.h>

void translate(WJElement ifaceoutput, WJElement ifaceinput, WJElement properties)
{
  WJElement property = NULL;
  while (property = _WJEObject(properties, "[]", WJE_GET, &property))
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
  WJElement properties = NULL;
  WJElement output = WJEObject(NULL, NULL, WJE_NEW);

  if (WJEGetF(schema,NULL,"%s.patternProperties",schemapath))
  {
    while (properties = WJEObjectF(schema, WJE_GET, &properties, "%s.patternProperties[]", schemapath))
    {
      while (ifaceinput = _WJEObject(input,"[]", WJE_GET, &ifaceinput))
      {
        ifaceoutput = WJEObject(output, ifaceinput->name, WJE_NEW);
        translate(ifaceoutput, ifaceinput, properties);
      }
    }
  }
  else if (WJEGetF(schema,NULL,"%s.properties",schemapath))
  {
    properties = WJEObject(schema, "%s.properties", WJE_GET);
    translate(output, WJEObject(input, "values", WJE_GET), properties);
  }
  return output;
}
