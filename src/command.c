#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "zcore.h"
#include "interpreter.h"
#include "command.h"
#include "utils.h"
#include "completion.h"
#include "option.h"
#include "acquire.h"
#include "face.h"

extern char **environ;
extern WJElement protojson;
extern WJElement protoface;
extern WJElement rl_protojson;
extern WJElement rl_parameter;
extern enum domains domain;

WJElement rl_commjson = NULL;
int rl_argcount = 0;

int listcommands(void)
{
  WJElement command = NULL;
  puts("Commands:");

  while ((command = _WJEObject(protojson, "schema.commands[]", WJE_GET, &command))) {
    puts(command->name);
  }
  return 0;
}

int iscommand(char * commandname)
{
  if (domain == FACE || domain == OPTION)
  {
    if (WJEGetF(protojson, NULL, "schema.commands.%s", commandname))
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
    if (WJEGetF(rl_protojson, NULL, "schema.commands.%s", commandname))
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

char faceenv[100] = "";

int setup_environment(char *envp[])
{
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
      sprintf(faceenv,"SECTION=%s", protoface->name);
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
  envp[i++] = NULL;
  return 0;
}

int command(int argc, char *argv[])
{
  char *myenv[100];
  setup_environment(myenv);

  char *args[100];
  WJElement command = WJEObjectF(protojson, WJE_GET, NULL, "schema.commands.%s", argv[0]);
  int argsc = arguments(WJEArray(command, "args", WJE_GET),args);

  if (WJEBool(command, "argpass", WJE_GET, 0) == TRUE)
  {
    argsc = argcat(argsc, args, &argv[1]);
  }

  if (strcmp(WJEString(command,"json", WJE_GET, "none"),"out") == 0)
  {
    streamfromcommand(WJEString(command, "command", WJE_GET, "/bin/false"),args,myenv,WJEArray(protojson, "data", WJE_GET));
  }
  else if (strcmp(WJEString(command,"json", WJE_GET, "none"),"in") == 0)
  {
    if (domain == OPTION)
    {
      streamintocommand(WJEString(command, "command", WJE_GET, "/bin/false"),args,myenv,WJEToString(protoface,TRUE));
    }
    else if (domain == FACE)
    {
      WJElement face = NULL;
      while (face = _WJEObject(protojson, "data[]", WJE_GET, &face)) {
        streamintocommand(WJEString(command, "command" ,WJE_GET, "/bin/false"),args,myenv,WJEToString(face,TRUE));
      }
    }
    else
    {
      puts("not implemented");
    }
  }
  else
  {
    if (WJEBool(command, "wait", WJE_GET, 0) == TRUE)
    {
      forkwaitexec(WJEString(command, "command", WJE_GET, "/bin/false"),argsc,args,myenv);
    }
    else
    {
      forkexec(WJEString(command, "command", WJE_GET, "/bin/false"),argsc,args,myenv);
    }
  }
  if (WJEBool(command, "reload", WJE_GET, FALSE) == TRUE)
  {
    //acquire(protojson);
    alarm(3);
  }
  return 1;
}

int rl_command(int argc, char *argv[])
{
  rl_commjson = WJEObjectF(rl_protojson, WJE_GET, NULL, "schema.commands.%s", argv[0]);
  rl_domain = COMMAND;
  rl_argcount = argc-1;
  return 1;
}

char * commandvalues(const char * text, int len)
{
  static WJElement command = NULL;
  while (command = _WJEObject(rl_protojson, "schema.commands[]", WJE_GET, &command)) {
    if (strncmp(command->name, text, len) == 0) {
      return strdup(command->name);
    }
  }
  return NULL;
}

char * cuecommandvalues(const char * text, int len, int state)
{
  //printf("arg num %d\n",rl_argcount);
  char * cuename = NULL;
  if (cuename = WJEStringF(rl_commjson, WJE_GET, NULL, NULL, "cue[%d]", rl_argcount))
  {
    //puts(cuename);
    rl_parameter = WJEObjectF(optionlist(rl_protojson), WJE_GET, NULL, "properties.%s",cuename);
    return cuesettingvalues(text, len, state);
  }
  else
  {
    return facevalues(text,len);
  }
  return NULL;
}
