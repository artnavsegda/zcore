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
    else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"object") == 0)
    {
      translate(WJEObject(ifaceoutput, property->name, WJE_NEW),WJEObject(ifaceinput, property->name, WJE_GET),WJEObject(property, "properties", WJE_GET));
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
    regex_t preg;
    while (properties = WJEObjectF(schema, WJE_GET, &properties, "%s.patternProperties[]", schemapath))
    {
      //puts(properties->name);
      regcomp(&preg, properties->name, REG_EXTENDED | REG_NOSUB);
      while (ifaceinput = _WJEObject(input,"[]", WJE_GET, &ifaceinput))
      {
        if (!regexec(&preg, ifaceinput->name, 0, NULL, 0))
        {
          ifaceoutput = WJEObject(output, ifaceinput->name, WJE_NEW);
          translate(ifaceoutput, ifaceinput, WJEObject(properties,"properties", WJE_GET));
        }
      }
      regfree(&preg);
    }
  }
  else if (WJEGetF(schema,NULL,"%s.properties",schemapath))
  {
    properties = WJEObjectF(schema, WJE_GET, NULL, "%s.properties", schemapath);
    translate(output, input, properties);
  }
  return output;
}
