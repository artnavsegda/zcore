#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement generator(char * generatedname, WJElement schema, char * schemapath)
{
  WJElement output = WJEObject(NULL, "generatedname", WJE_NEW);
  WJElement property = NULL;

  WJEString(output,"name",WJE_NEW,generatedname);

  /*while (property = _WJEObject(schema,schemapath, WJE_GET, &property))
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
  }*/

  return output;
}
