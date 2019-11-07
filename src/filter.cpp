#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

json_object * translate(json_object * properties, json_object * input)
{
  json_object * output = json_object_new_object();
  json_object_object_foreach(properties, key, val)
  {
    json_object * type = NULL;
    char * typestring;
    json_object_object_get_ex(val, "type", &type);
    typestring = (char *)json_object_get_string(type);
    json_object * entity = NULL;

    if (json_object_object_get_ex(input, key, &entity))
    {
      if (strcmp(typestring,"string") == 0)
      {
        json_object_object_add(output, key, entity);
      }
      else if (strcmp(typestring,"number") == 0)
      {
        json_object_object_add(output, key, json_object_new_int(atoi(json_object_get_string(entity))));
      }
      else if (strcmp(typestring,"boolean") == 0)
      {
        //json_object_object_add(output, key, NULL);
      }
      else if (strcmp(typestring,"array") == 0)
      {
        //json_object_object_add(output, key, entity);
        //json_object * newarray = json_object_new_array();
        //json_object_object_add(output, key, newarray);
        //json_object_deep_copy(entity, &output, NULL);
      }
    }
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
      //puts(key);
      output = json_object_new_object();
      json_object_object_get_ex(val, "properties", &properties);
      json_object_object_foreach(input, key, val)
      {
        //puts(json_object_to_json_string_ext(properties, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
        //puts(json_object_to_json_string_ext(val, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
        json_object_object_add(output, key, translate(properties, val));
        //output = translate(properties, val);
      }
    }
  }
  else if (!json_pointer_getf(schema, &properties, "/%s/schema/properties", schemapath))
  {
      //puts(json_object_to_json_string_ext(properties, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
      //puts(json_object_to_json_string_ext(input, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
      output = translate(properties, input);
  }

  return output;
}
