#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <json-c/json.h>
#include "interpreter.h"
#include "builtin.h"

extern json_object * protojson;
extern json_object * protoface;
extern int protodepth;
extern enum domains domain;
extern path_t * path;

char * parentname(path_t * element, int depth)
{
  for (int i = 1; i < depth; i++)
  {
    element = element->parent;
  }
  return element->name;
}

void generateprompt(char * prompt)
{
  char hostname[HOST_NAME_MAX];
  gethostname(hostname, HOST_NAME_MAX);
//  sprintf(prompt, "[%s@%s]/%s>", getlogin(), hostname, protoname);
  prompt[0] = '\0';
  strcat(prompt, "[");
  strcat(prompt, getlogin());
  strcat(prompt, "@");
  strcat(prompt, hostname);
  strcat(prompt, "]");

  strcat(prompt, "/");
  strcat(prompt, path->name);

//   for (int i = protodepth; i > 0; i--)
//   {
//     strcat(prompt, "/");
//     strcat(prompt, parentname(protojson, i));
//   }
//   if (domain == OPTION)
//   {
//     if (strcmp(WJEString(protojson,"schema.type",WJE_GET,"unknown"),"array") == 0)
//     {
//       strcat(prompt, "/");
//       strcat(prompt, WJEString(protoface, "name", WJE_GET, ""));
//     }
//   }
  strcat(prompt, ">");
}
