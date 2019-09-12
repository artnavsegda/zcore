#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement generator(char * generatedname, WJElement schema, char * schemapath)
{
  char * itemname;
  WJElement output = WJEObject(NULL, "generatedname", WJE_NEW);
  WJElement property = NULL;
  WJElement reqstring = NULL;

  WJEString(output,"name",WJE_NEW,generatedname);

  //WJEDump(schema);

  while (itemname = WJEStringF(schema, WJE_GET, &reqstring, NULL, "%s.required[]", schemapath))
  {
    //puts(itemname);
    property = WJEObjectF(schema, WJE_GET, NULL, "ethernet.schema.items.properties.%s", itemname);
    //WJEDump(property);

    if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"string") == 0)
    {
      char * stringvalue = WJEString(property,"example",WJE_GET,"");
      if (stringvalue[0])
      {
        WJEString(output,property->name,WJE_NEW,stringvalue);
      }
    }
    else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"number") == 0)
    {
      int numbervalue = WJEInt32(property,"example",WJE_GET,-1);
      if (numbervalue != -1)
      {
        WJEInt32(output,property->name,WJE_NEW,numbervalue);
      }
    }
    else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"boolean") == 0)
    {
      int boolval = WJEInt32(property,"example",WJE_GET,-1);
      if (boolval != -1)
      {
        WJEBool(output,property->name,WJE_NEW,(XplBool)boolval);
      }
    }
  }

  return output;
}
