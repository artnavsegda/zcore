#include <stdio.h>
#include <string.h>
#include "builtin.h"
#include "interpreter.h"
#include "proto.h"
#include "zcore.h"
#include "face.h"
#include "command.h"
#include "option.h"
#include "prompt.h"
#include "acquire.h"
#include "utils.h"
#include "global.h"
#include "validate.h"
#include "setting.h"
#include "interpreter.h"
#include "completion.h"

extern int protodepth;
extern int rl_protodepth;
extern WJElement protojson;
extern WJElement rl_protojson;
extern WJElement protoface;
extern WJElement protoschema;
extern enum domains domain;
extern enum domains rl_domain;
extern int optiondepth;

int path_up(int argc, char *argv[])
{
  switch (domain)
  {
    case PROTO:
      if (protodepth == 0)
      {
        printf("Already at the command root\n");
      }
      else
      {
        protodepth--;
        protojson = protojson->parent;
      }
    break;
    case FACE:
      domain = PROTO;
      protodepth--;
      protojson = protojson->parent;
    break;
    case OPTION:
      if (optiondepth > 0)
      {
        optiondepth--;
        protoface = protoface->parent;
        protoschema = protoschema->parent->parent;
      }
      else
      {
        if (WJEGet(protojson, "schema.patternProperties", NULL))
        {
          domain = FACE;
        }
        else if (WJEGet(protojson, "schema.properties", NULL))
        {
          domain = PROTO;
          protodepth--;
          protojson = protojson->parent;
        }
      }
    break;
  }
  if (argc > 1)
    return execute(argc-1, &argv[1]);
  else
    return 0;
}

int rl_path_up(int argc, char *argv[])
{
  switch (rl_domain)
  {
    case PROTO:
      if (rl_protodepth == 0)
      {
        //printf("Already at the command root\n");
      }
      else
      {
        rl_protodepth--;
        rl_protojson = protojson->parent;
      }
    break;
    case FACE:
      rl_domain = PROTO;
      rl_protodepth--;
      rl_protojson = protojson->parent;
    break;
    case OPTION:
      // if (optiondepth > 0)
      // {
      //   optiondepth--;
      //   protoface = protoface->parent;
      //   protoschema = protoschema->parent->parent;
      // }
      // else
      // {
        if (WJEGet(protojson, "schema.patternProperties", NULL))
        {
          rl_domain = FACE;
        }
        else if (WJEGet(protojson, "schema.properties", NULL))
        {
          rl_domain = PROTO;
          rl_protodepth--;
          rl_protojson = rl_protojson->parent;
        }
      // }
    break;
  }
  if (argc > 1)
    return rl_execute(argc-1, &argv[1]);
  else
    return 0;
}

int path_root(int argc, char *argv[])
{
  domain = PROTO;
  protodepth = 0;
  protojson = root;
  if (argc > 1)
    return execute(argc-1, &argv[1]);
  else
    return 0;
}

int rl_path_root(int argc, char *argv[])
{
  rl_domain = PROTO;
  rl_protodepth = 0;
  rl_protojson = root;
  if (argc > 1)
    return rl_execute(argc-1, &argv[1]);
  else
    return 0;
  return 0;
}

int commandlist(int argc, char *argv[])
{
  puts("tokens:");
  listglobals(argc-1, &argv[1]);
  listbuiltins(argc-1, &argv[1]);
  switch (domain)
  {
    case PROTO:
      listprotos();
    break;
    case FACE:
      listfaces();
      listcommands();
    break;
    case OPTION:
      listoptions();
    break;
  }
  return 1;
}

char *builtincommands[] = {"show","acquire","validate","export","list",NULL};

void listbuiltins(int argc, char *argv[])
{
  puts("builtins:");
  puts("list");
//  puts("..");
//  puts("/");
  puts("show");
  puts("acquire");
  puts("validate");
  puts("export");
}

int isbuiltin(char * builtinname)
{
  if ((strcmp(builtinname,"list") == 0) ||
  (strcmp(builtinname,"..") == 0) ||
  (strcmp(builtinname,"show") == 0) ||
  (strcmp(builtinname,"acquire") == 0) ||
  (strcmp(builtinname,"/") == 0) ||
  (strcmp(builtinname,"validate") == 0) ||
  (strcmp(builtinname,"export") == 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int rl_isbuiltin(char * builtinname)
{
  if ((strcmp(builtinname,"list") == 0) ||
  (strcmp(builtinname,"..") == 0) ||
  (strcmp(builtinname,"show") == 0) ||
  (strcmp(builtinname,"acquire") == 0) ||
  (strcmp(builtinname,"/") == 0) ||
  (strcmp(builtinname,"validate") == 0) ||
  (strcmp(builtinname,"export") == 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

extern WJElement optionlistone;

int printoption(WJElement proto, WJElement face, int depth, WJElement schema)
{
  WJElement option = NULL;
  char * facename = NULL;
  if (face)
    facename = face->name;
  else
    facename = NULL;

  static WJElement optionlistthree = NULL;

    if (optionlistthree)
    {
      WJECloseDocument(optionlistthree);
      optionlistthree = NULL;
    }
    optionlistthree = anyoption(schema, face);

  //puts("DEBUG237");
  //WJEDump(optionlistone);

  while (option = _WJEObject(optionlistthree, "properties[]", WJE_GET, &option)) {
    char stringbuffer[1000] = "";
    if (!WJEBool(option, "hidden", WJE_GET, FALSE))
    {
      for (int i = depth; i > 0; i--)
      {
        sprintf(stringbuffer, "%s%s.", stringbuffer, parentname(proto, i));
      }
      if (WJEGet(proto, "schema.patternProperties", NULL))
      {
        if (optiondepth > 0)
          sprintf(stringbuffer, "%s%s.", stringbuffer, face->parent->name);
        sprintf(stringbuffer, "%s%s.", stringbuffer, elementname(proto,face));
      }
      sprintf(stringbuffer, "%s%s", stringbuffer, option->name);

      WJElement tempoption = conditionoption(schema, face, option->name);

      //puts("DEBUG255");
      //WJEDump(tempoption);

      if (strcmp(WJEString(tempoption,"type", WJE_GET, NULL),"object") == 0){
        WJElement suboption = NULL;
        while (suboption = _WJEObject(option, "properties[]", WJE_GET, &suboption)) {
          printf(stringbuffer);
          printf(".%s", suboption->name);
          printf(" = ");

          char * returnstring = optionvalue(suboption->name, option, WJEGet(face, option->name, NULL));
          if (returnstring)
          {
            puts(returnstring);
            free(returnstring);
          }
          else
            puts("None");
        }
      }
      else
      {
        printf(stringbuffer);
        printf(" = ");
        char * returnstring = optionvalue(option->name, schema, face);
        if (returnstring)
        {
          puts(returnstring);
          free(returnstring);
        }
        else
          puts("None");
      }
    }
  }
  return 0;
}

int printoption2(WJElement proto, int depth)
{
  WJElement face = NULL;
  while (face = _WJEObject(proto, "data[]", WJE_GET, &face)) {
    printoption(proto,face, depth, WJEGet(proto, "schema", NULL));
  }
  return 0;
}

int printoption3(WJElement protoinput, int depth)
{
  WJElement proto = NULL;
  depth++;
  while ((proto = _WJEObject(protoinput, "[]", WJE_GET, &proto))) {
    if (WJEGet(proto, "schema", NULL))
    {
      if (!WJEBool(proto, "schema.hidden", WJE_GET, FALSE))
      {
        if (WJEGet(proto, "schema.patternProperties", NULL))
        {
          printoption2(proto, depth);
        }
        else if (WJEGet(proto, "schema.properties", NULL))
        {
          printoption(proto,WJEGet(proto, "data", NULL),depth, WJEGet(proto, "schema", NULL));
        }
      }
    }
    else
    {
      printoption3(proto, depth);
    }
  }
  return 0;
}

int builtin_show(int argc, char *argv[])
{
  if (domain == OPTION)
  {
    printoption(protojson,protoface,protodepth,protoschema);
  }
  else if (domain == FACE)
  {
    printoption2(protojson, protodepth);
  }
  else if (domain == PROTO)
  {
    if (protodepth == 0)
      protojson = root;
    printoption3(protojson,protodepth);
  }
  else
    puts("Not implemented");
  return 1;
}

int exportoption(WJElement proto, WJElement face, int depth, WJElement schema)
{
  WJElement option = NULL;
  char * facename = NULL;
  if (face)
    facename = face->name;
  else
    facename = NULL;

    static WJElement optionlistthree = NULL;

      if (optionlistthree)
      {
        WJECloseDocument(optionlistthree);
        optionlistthree = NULL;
      }
      optionlistthree = anyoption(schema, face);

  while (option = _WJEObject(optionlistthree, "properties[]", WJE_GET, &option)) {
    char stringbuffer[1000] = "";
    if (!WJEBool(option, "hidden", WJE_GET, FALSE))
    {
      for (int i = depth; i > 0; i--)
      {
        sprintf(stringbuffer, "%s%s ", stringbuffer, parentname(proto, i));
      }
      if (WJEGet(proto, "schema.patternProperties", NULL))
      {
        if (optiondepth > 0)
          sprintf(stringbuffer, "%s%s ", stringbuffer, face->parent->name);
        sprintf(stringbuffer, "%s%s ", stringbuffer, elementname(proto,face));
      }
      sprintf(stringbuffer, "%s%s", stringbuffer, option->name);

      WJElement tempoption = conditionoption(schema, face, option->name);

      if (strcmp(WJEString(tempoption,"type", WJE_GET, NULL),"object") == 0){
        WJElement suboption = NULL;
        while (suboption = _WJEObject(option, "properties[]", WJE_GET, &suboption)) {
          printf(stringbuffer);
          printf(" %s ", suboption->name);

          char * returnstring = optionvalue(suboption->name, option, WJEGet(face, option->name, NULL));
          if (returnstring)
          {
            puts(returnstring);
            free(returnstring);
          }
          else
            puts("-");
        }
      }
      else
      {
        printf(stringbuffer);
        printf(" ");
        char * returnstring = optionvalue(option->name, schema, face);
        if (returnstring)
        {
          puts(returnstring);
          free(returnstring);
        }
        else
          puts("-");
      }
    }
  }
  return 0;
}

int exportoption2(WJElement proto, int depth)
{
  WJElement face = NULL;
  while (face = _WJEObject(proto, "data[]", WJE_GET, &face)) {
    exportoption(proto,face, depth, WJEGet(proto, "schema", NULL));
  }
  return 0;
}

int exportoption3(WJElement protoinput, int depth)
{
  WJElement proto = NULL;
  depth++;
  while ((proto = _WJEObject(protoinput, "[]", WJE_GET, &proto))) {
    if (WJEGet(proto, "schema", NULL))
    {
      if (!WJEBool(proto, "schema.hidden", WJE_GET, FALSE))
      {
        if (WJEGet(proto, "schema.patternProperties", NULL))
        {
          exportoption2(proto, depth);
        }
        else if (WJEGet(proto, "schema.properties", NULL))
        {
          exportoption(proto,WJEObject(proto, "data", WJE_GET),depth, WJEGet(proto, "schema", NULL));
        }
      }
    }
    else
    {
      exportoption3(proto, depth);
    }
  }
  return 0;
}

int builtin_export(int argc, char *argv[])
{
  if (domain == OPTION)
  {
    exportoption(protojson,protoface,protodepth,protoschema);
  }
  else if (domain == FACE)
  {
    exportoption2(protojson, protodepth);
  }
  else if (domain == PROTO)
  {
    if (protodepth == 0)
      protojson = root;
    exportoption3(protojson,protodepth);
  }
  else
    puts("Not implemented");
  return 1;
}

int builtin_acquire(int argc, char *argv[])
{
  acquire(WJEObject(root, argv[1], WJE_GET));
  return 1;
}

int builtin_validate(int argc, char *argv[])
{
  if (domain == FACE || domain == OPTION)
  {
    puts("schema:");
    WJEDump(WJEGet(protojson,"schema",NULL));
    puts("data:");
    WJEDump(WJEGet(protojson,"data",NULL));

    WJElement tempschema = WJEObject(NULL, NULL, WJE_NEW);
    WJECopyDocument(tempschema, WJEGet(protojson,"schema",NULL), NULL, NULL);

    WJElement tempdata = WJEObject(NULL, NULL, WJE_NEW);
    WJECopyDocument(tempdata, WJEGet(protojson,"data",NULL), NULL, NULL);

    if (WJESchemaValidate(tempschema, tempdata, schema_error, schema_load, schema_free, "%s"))
    {
      puts("schema valid");
    }
    else
    {
      puts("schema invalid");
    }

    WJECloseDocument(tempschema);
    WJECloseDocument(tempdata);
  }
  return 1;
}

int builtin(int argc, char *argv[])
{
  if (strcmp(argv[0],"list") == 0)
  {
    return commandlist(argc,argv);
  }
  else if (argv[0][0]=='/')
  {
    return path_root(argc,argv);
  }
  else if (strcmp(argv[0],"..") == 0)
  {
    return path_up(argc,argv);
  }
  else if (strcmp(argv[0],"show") == 0)
  {
    return builtin_show(argc,argv);
  }
  else if (strcmp(argv[0],"acquire") == 0)
  {
    return builtin_acquire(argc,argv);
  }
  else if (strcmp(argv[0],"validate") == 0)
  {
    return builtin_validate(argc,argv);
  }
  else if (strcmp(argv[0],"export") == 0)
  {
    return builtin_export(argc,argv);
  }
  return 0;
}

int rl_builtin(int argc, char *argv[])
{
  if (argv[0][0]=='/')
  {
    return rl_path_root(argc,argv);
  }
  else if (strcmp(argv[0],"..") == 0)
  {
    return rl_path_up(argc,argv);
  }
  return 0;
}

char * builtinvalues(const char * text, int len)
{
  static int valueindex = 0;

  while (builtincommands[valueindex] != NULL)
  {
    if (strncmp(builtincommands[valueindex], text, len)==0)
    {
      return strdup(builtincommands[valueindex++]);
    }
    else
    {
      valueindex++;
    }
  }
  valueindex = 0;
  return NULL;
}
