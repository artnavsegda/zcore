#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <wjelement.h>
#include "interpreter.h"
#include "utils.h"
#include "domain.h"

extern WJElement protojson;
extern WJElement protoface;
extern int protodepth;
extern enum domains domain;
extern int optiondepth;

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
  char * loginname = getlogin();
  if (loginname)
  	strcat(prompt, loginname);
  else
	strcat(prompt, "nologin");
  strcat(prompt, "@");
  strcat(prompt, hostname);
  strcat(prompt, "]");
  for (int i = protodepth; i > 0; i--)
  {
    strcat(prompt, "/");
    strcat(prompt, parentname(protojson, i));
  }
  if (domain == OPTION)
  {
    for (int i = optiondepth; i > 0; i--)
    {
      strcat(prompt, "/");
      strcat(prompt, parentname(protoface, i+1));
    }
    if (WJEGet(protojson, "schema.patternProperties", NULL))
    {
      strcat(prompt, "/");
      strcat(prompt, elementname(protojson,protoface));
    }
    //if (optiondepth > 0)
    //{
    //  strcat(prompt, "/");
    //  strcat(prompt, protoface->name);
    //}
  }
  strcat(prompt, ">");
}
