#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement generator(WJElement schema, char * schemapath, char * commandname, int argc, char *argv[])
{
  char * itemname;
  WJElement output = WJEObject(NULL, "generatedname", WJE_NEW);
  WJElement property = NULL;
  WJElement reqstring = NULL;

  //WJEString(output,"name",WJE_NEW,generatedname);

  while (itemname = WJEStringF(schema, WJE_GET, &reqstring, NULL, "%s.items.required[]", schemapath))
  {
    property = WJEObjectF(schema, WJE_GET, NULL, "%s.items.properties.%s", schemapath, itemname);

    if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"string") == 0)
    {
      char * stringvalue = WJEString(property,"default",WJE_GET,"");
      if (stringvalue[0])
      {
        WJEString(output,property->name,WJE_NEW,stringvalue);
      }
    }
    else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"number") == 0)
    {
      int numbervalue = WJEInt32(property,"default",WJE_GET,-1);
      if (numbervalue != -1)
      {
        WJEInt32(output,property->name,WJE_NEW,numbervalue);
      }
    }
    else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"boolean") == 0)
    {
      int boolval = WJEInt32(property,"default",WJE_GET,-1);
      if (boolval != -1)
      {
        WJEBool(output,property->name,WJE_NEW,(XplBool)boolval);
      }
    }
    else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"array") == 0)
    {
      WJElement myarray = WJEArray(output,property->name,WJE_NEW);
      if (strcmp(WJEString(property,"items.type",WJE_GET,"unknown"),"string") == 0)
      {
        char * stringvalue = WJEString(property,"items.default",WJE_GET,"");
        if (stringvalue[0])
        {
          WJEString(myarray,property->name,WJE_NEW,stringvalue);
        }
      }
    }
  }



  return output;
}
