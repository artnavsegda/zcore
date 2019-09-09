#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <wjelement.h>

extern WJElement protojson;
extern int protodepth;

char * parentname(WJElement element, int depth)
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
  for (int i = protodepth; i > 0; i--)
  {
    strcat(prompt, "/");
    strcat(prompt, parentname(protojson, i));
  }
  strcat(prompt, ">");
}
