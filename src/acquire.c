#include <stdio.h>
#include <json-c/json.h>
#include "utils.h"

int acquire(json_object * proto)
{
  json_object * readjson;
  int jsonstream;
  char *argv[100];
  json_object * listargs = NULL;
  json_pointer_get(proto, "/schema/acquire/args", &listargs);
  int argc = arguments(listargs, argv);

  json_object * jsoncommand = NULL;
  json_pointer_get(proto, "/schema/acquire/shell", &jsoncommand);

  if (!(jsonstream = my_popen_read(json_object_get_string(jsoncommand), argv,  NULL))) {
    puts("handle error");
    return 1;
  }

  if ((readjson = json_object_from_fd(jsonstream)) == NULL) {
    puts("json failed to open");
    return 1;
  }

  json_object_object_add(proto, "data", readjson);
}

int acquireall(json_object * directory)
{
  json_object_object_foreach(directory, key, val)
  {
    if(json_object_object_get_ex(val,"schema",NULL))
    {
      acquire(val);
    }
    else
    {
      acquireall(val);
    }
  }
}
