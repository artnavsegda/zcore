#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <wjelement.h>
#include <wjreader.h>
#include "validate.h"

void translate(WJElement ifaceoutput, WJElement ifaceinput, WJElement properties, WJElement schema);

void translateproperty(WJElement ifaceoutput, WJElement ifaceinput, WJElement property, WJElement schema)
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
    translate(WJEObject(ifaceoutput, property->name, WJE_NEW),WJEObject(ifaceinput, property->name, WJE_GET),property, schema);
  }
}

void translate(WJElement ifaceoutput, WJElement ifaceinput, WJElement properties, WJElement schema)
{

  WJElement property = NULL;
  while (property = _WJEObject(properties, "properties[]", WJE_GET, &property))
  {
    char * refpath = NULL;
    refpath = WJEString(property, "[\"$ref\"]", WJE_GET, NULL);
    if (refpath)
    {
      char *ptr = strrchr (refpath, '/');
      if (ptr) {
        ++ptr;
        WJElement schema_definitions = WJEObject(schema, "definitions", WJE_GET);
        if (schema_definitions)
        {
          WJElement sub = WJEObject(schema_definitions, ptr, WJE_GET);
          if (sub)
          {
            translateproperty(ifaceoutput, ifaceinput, sub, schema);
          }
        }
      }
    }
    else
      translateproperty(ifaceoutput, ifaceinput, property, schema);
  }
}

WJElement filter(WJElement input, WJElement schema, char * schemapath)
{
  WJElement ifaceinput = NULL, ifaceoutput = NULL;
  WJElement properties = NULL;
  WJElement output = WJEObject(NULL, NULL, WJE_NEW);
  WJElement schemaroot = WJEGet(schema,schemapath,NULL);

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
          translate(ifaceoutput, ifaceinput, properties, schemaroot);
        }
      }
      regfree(&preg);
    }
  }
  else
  {
    properties = WJEObject(schema, schemapath, WJE_GET);
    translate(output, input, properties, schemaroot);
  }
  return output;
}
