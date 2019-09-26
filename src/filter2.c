#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement filter(WJElement input, WJElement schema, char * schemapath, char * ubustype, char * ubusconfig)
{
  WJElement output = WJEObject(NULL, NULL, WJE_NEW);
  WJElement property = NULL;
  WJElement valuesoutput = NULL;

  WJEString(output,"config",WJE_NEW,ubusconfig);

  WJEString(output,"type",WJE_NEW,ubustype);

  char * name = WJEString(input,"section",WJE_GET,NULL);
  if (name){
    WJEString(output,"name",WJE_NEW,name);
  }

  valuesoutput = WJEObject(output, "values", WJE_NEW);

    while (property = WJEObjectF(schema, WJE_GET, &property, "%s.items.properties[]", schemapath))
    {
      if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"string") == 0)
      {
        char * stringvalue = WJEString(input,property->name,WJE_GET,NULL);
        if (stringvalue && strcmp(property->name,"section"))
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
      else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"array") == 0)
      {
        WJEAttach(valuesoutput,WJEArray(input, property->name, WJE_GET));
      }
    }

  return output;
}
