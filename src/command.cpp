#include <stdio.h>
#include <string.h>
//#include "zcore.h"
#include "interpreter.h"
#include "command.h"
#include "utils.h"
#include "completion.h"
#include "option.h"
#include "acquire.h"
#include "face.h"

extern char **environ;
extern json_object * protojson;
extern json_object * protoface;
extern json_object * rl_protojson;
extern json_object * rl_parameter;

extern char facename[100];

// WJElement rl_commjson = NULL;
int rl_argcount = 0;

int listcommands(void)
{
  json_object * commands = NULL;

  if (!json_pointer_get(protojson, "/schema/commands", &commands))
  {
    puts("Commands:");
    json_object_object_foreach(commands, key, val)
    {
      puts(key);
    }
  }

  return 0;
}

int iscommand(char * commandname)
{
  if (domain == FACE || domain == OPTION)
  {
    if (!json_pointer_getf(protojson, NULL, "/schema/commands/%s", commandname))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

int rl_iscommand(char * commandname)
{
  if (domain == FACE || domain == OPTION)
  {
    if (!json_pointer_getf(rl_protojson, NULL, "/schema/commands/%s", commandname))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

int command(int argc, char *argv[])
{
  char *envp[100];

  char *args[100];
  json_object * command = NULL;
  json_pointer_getf(protojson, &command, "/schema/commands/%s", argv[0]);
  json_object * jsargs = NULL;
  json_object_object_get_ex(command, "args", &jsargs);
  int argsc = arguments(jsargs,args);

  char faceenv[100] = "";
  int i = 0;
  //clearenv();
  switch(domain)
  {
    case PROTO:
      envp[i++] = "DOMAIN=PROTO";
      //setenv("DOMAIN", "PROTO", 1);
      //myenv[0] = "DOMAIN=PROTO";
      //myenv[1] = NULL;
    break;
    case FACE:
      envp[i++] = "DOMAIN=FACE";
      //setenv("DOMAIN", "FACE", 1);
      //setenv("DOM", "FA", 1);
      //myenv[0] = "DOMAIN=FACE";
      //myenv[1] = NULL;
    break;
    case OPTION:
      envp[i++] = "DOMAIN=OPTION";
      sprintf(faceenv,"SECTION=%s", facename);
      envp[i++] = faceenv;
      //setenv("DOMAIN", "OPTION", 1);
      //setenv("FACE", elementname(protojson,protoface), 1);
      //setenv("FACE", "test1", 1);
      //fillenv(protojson,protoface);
      //myenv[0] = "DOMAIN=OPTION";
      //myenv[1] = facename;
      //myenv[2] = NULL;
    break;
    case SETTING:
      envp[i++] = "DOMAIN=OPTION";
      //setenv("DOMAIN", "SETTING", 1);
      //myenv[0] = "DOMAIN=SETTING";
      //myenv[1] = NULL;
    break;
    case COMMAND:
      envp[i++] = "DOMAIN=OPTION";
      //setenv("DOMAIN", "COMMAND", 1);
      //myenv[0] = "DOMAIN=COMMAND";
      //myenv[1] = NULL;
    break;
  }

  char cuestring[100] = "CUE=";
  char * cuename = NULL;
  json_object * cue = NULL;
  json_object_object_get_ex(command, "cue", &cue);

  for(int ii = 0; ii < json_object_array_length(cue); ii++)
  {
    strcat(cuestring, json_object_to_json_string(json_object_array_get_idx(cue, ii)));
    strcat(cuestring, " ");
  }
  envp[i++] = cuestring;
  envp[i++] = NULL;

  json_object * argpass = NULL;
  json_object_object_get_ex(command, "argpass", &argpass);

  if (json_object_get_boolean(argpass))
  {
    argsc = argcat(argsc, args, &argv[1]);
  }

  json_object * json = NULL;
  json_object_object_get_ex(command, "json", &json);

  json_object * data = NULL;
  json_object_object_get_ex(command, "data", &data);

  json_object * commandname = NULL;
  json_object_object_get_ex(command, "command", &commandname);

  if (strcmp(json_object_get_string(json),"out") == 0)
  {
    streamfromcommand((char *)json_object_to_json_string(commandname),args,envp,data);
  }
  else if (strcmp(json_object_get_string(json),"in") == 0)
  {
    if (domain == OPTION)
    {
      streamintocommand((char *)json_object_to_json_string(commandname),args,envp,WJEToString(protoface,TRUE));
    }
  //   else if (domain == FACE)
  //   {
  //     WJElement face = NULL;
  //     while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
  //       streamintocommand(WJEString(command, "command" ,WJE_GET, "/bin/false"),args,envp,WJEToString(face,TRUE));
  //     }
  //   }
  //   else
  //   {
  //     puts("not implemented");
  //   }
  }
  // else
  // {
  //   if (WJEBool(command, "wait", WJE_GET, 0) == TRUE)
  //   {
  //     forkwaitexec(WJEString(command, "command", WJE_GET, "/bin/false"),argsc,args,envp);
  //   }
  //   else
  //   {
  //     forkexec(WJEString(command, "command", WJE_GET, "/bin/false"),argsc,args,envp);
  //   }
  // }
  // if (WJEBool(command, "reload", WJE_GET, FALSE) == TRUE)
  // {
  //   //acquire(protojson);
  //   alarm(3);
  // }
  return 1;
}

int rl_command(int argc, char *argv[])
{
//   rl_commjson = WJEObjectF(rl_protojson, WJE_GET, NULL, "schema.commands.%s", argv[0]);
//   rl_domain = COMMAND;
//   rl_argcount = argc-1;
  return 1;
}

char * commandvalues(const char * text, int len)
{
  static lh_entry * entry = NULL;
  if (entry == NULL)
  {
    json_object * commands = NULL;
    if (!json_pointer_get(rl_protojson, "/schema/commands", &commands))
      entry = json_object_get_object(commands)->head;
    else
      return NULL;
  }
  else
    entry = entry->next;
  while (entry)
  {
    char * keyname = (char*)lh_entry_k(entry);
    if (strncmp(keyname, text, len) == 0) {
      return strdup(keyname);
    }
    else
      entry = entry->next;
  }
  return NULL;
}

char * cuecommandvalues(const char * text, int len, int state)
{
//   //printf("arg num %d\n",rl_argcount);
//   char * cuename = NULL;
//   if (cuename = WJEStringF(rl_commjson, WJE_GET, NULL, NULL, "cue[%d]", rl_argcount))
//   {
//     //puts(cuename);
//     rl_parameter = WJEObjectF(optionlist(rl_protojson), WJE_GET, NULL, "properties.%s",cuename);
//     return cuesettingvalues(text, len, state);
//   }
//   else
//   {
//     return facevalues(text,len);
//   }
  return NULL;
}
