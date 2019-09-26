#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wjelement.h>
#include <wjreader.h>

WJElement generator(WJElement schema,  char * schemapath, char * commandname, char * ubustype, char * ubusconfig, int argc, char *argv[])
{
  char * itemname;
  WJElement alloutput = WJEObject(NULL, "generatedname", WJE_NEW);
  WJElement property = NULL;
  WJElement reqstring = NULL;
  WJElement valuesoutput = NULL;

  WJEString(alloutput,"config",WJE_NEW,ubusconfig);
  WJEString(alloutput,"type",WJE_NEW,ubustype);

  WJElement output = WJEObject(alloutput, "values", WJE_NEW);

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

  char * cuename = NULL;

  for (int i = 0; i < argc; i++)
  {
    if (cuename = WJEStringF(schema, WJE_GET, NULL, NULL, "%s.commands.%s.cue[%d]", schemapath, commandname, i))
    {
      property = WJEObjectF(schema, WJE_GET, NULL, "%s.items.properties.%s", schemapath, cuename);
      if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"string") == 0)
      {
        WJEString(output,property->name,WJE_SET,argv[i]);
      }
      else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"number") == 0)
      {
        WJEInt32(output,property->name,WJE_SET,atoi(argv[i]));
      }
    }
  }

  return alloutput;
}
