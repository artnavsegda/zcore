#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

// void translate(WJElement ifaceoutput, WJElement ifaceinput, WJElement schema, char * schemapath, char * protopath)
// {
//   WJElement property = NULL;
//   while (property = WJEObjectF(schema, WJE_GET, &property, protopath, schemapath))
//   {
//     if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"string") == 0)
//     {
//       char * stringvalue = WJEString(ifaceinput,property->name,WJE_GET,"");
//       if (stringvalue[0])
//       {
//         WJEString(ifaceoutput,property->name,WJE_NEW,stringvalue);
//       }
//     }
//     else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"number") == 0)
//     {
//       int numbervalue = WJEInt32(ifaceinput,property->name,WJE_GET,-1);
//       if (numbervalue != -1)
//       {
//         WJEInt32(ifaceoutput,property->name,WJE_NEW,numbervalue);
//       }
//     }
//     else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"boolean") == 0)
//     {
//       int boolval = WJEInt32(ifaceinput,property->name,WJE_GET,-1);
//       if (boolval != -1)
//       {
//         WJEBool(ifaceoutput,property->name,WJE_NEW,(XplBool)boolval);
//       }
//     }
//     else if (strcmp(WJEString(property,"type",WJE_GET,"unknown"),"array") == 0)
//     {
//       WJEAttach(ifaceoutput,WJEArray(ifaceinput, property->name, WJE_GET));
//     }
//   }
// }

json_object * translate(json_object * properties, json_object * input)
{
  json_object * output = NULL;
  json_object_object_foreach(properties, key, val)
  {
    json_object * type = NULL;
    char * typestring;
    json_object_object_get_ex(val, "type", &type);
    puts(key);
    //puts(json_object_to_json_string_ext(val, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
    puts(json_object_to_json_string_ext(type, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
    json_object * entity = NULL;
    json_object_object_get_ex(input, key, &entity);
    typestring = json_object_to_json_string_ext(entity, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
    puts(typestring);

    //json_object_object_add(output, key, val);

  }
  return output;
}

json_object * filter(json_object * input, json_object * schema, char * schemapath)
{
  json_object * properties = NULL;
  json_object * patternProperties = NULL;
  json_object * output = NULL;

  if (!json_pointer_getf(schema, &patternProperties, "/%s/schema/patternProperties", schemapath))
  {
    json_object_object_foreach(patternProperties, key, val)
    {
      puts(key);
      json_object_object_get_ex(val, "properties", &properties);
      json_object_object_foreach(input, key, val)
      {
        puts(json_object_to_json_string_ext(properties, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
        puts(json_object_to_json_string_ext(val, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
        //output = translate(properties, input);
      }
    }
  }
  else if (!json_pointer_getf(schema, &properties, "/%s/schema/properties", schemapath))
  {
      //puts(json_object_to_json_string_ext(properties, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
      //puts(json_object_to_json_string_ext(input, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
      output = translate(properties, input);
  }

  // WJElement ifaceinput = NULL, ifaceoutput = NULL;
  //
  // if (WJEGetF(schema,NULL,"%s.patternProperties",schemapath))
  // {
  //   WJElement output = WJEObject(NULL, NULL, WJE_NEW);
  //   while (ifaceinput = _WJEObject(input,"[]", WJE_GET, &ifaceinput))
  //   {
  //     ifaceoutput = WJEObject(output, ifaceinput->name, WJE_NEW);
  //     translate(ifaceoutput, ifaceinput, schema, schemapath, "%s.patternProperties[0].properties[]");
  //   }
  //   return output;
  // }
  // else if (WJEGetF(schema,NULL,"%s.properties",schemapath))
  // {
  //   WJElement output = WJEObject(NULL, NULL, WJE_NEW);
  //   translate(output, WJEObject(input, "values", WJE_GET), schema, schemapath, "%s.properties[]");
  //   return output;
  // }
  return output;
}
