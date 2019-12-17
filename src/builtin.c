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
#include "domain.h"
#include "global.h"

extern int protodepth;
extern WJElement protojson;
extern WJElement protoface;
extern WJElement protoschema;
extern enum domains domain;
extern int optiondepth;

int path_up()
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
  return 0;
}

int path_root()
{
  domain = PROTO;
  protodepth = 0;
  protojson = root;
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
      listcommands();
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
  if ((strcmp(builtinname,"list") == 0) || (strcmp(builtinname,"..") == 0) || (strcmp(builtinname,"show") == 0) || (strcmp(builtinname,"acquire") == 0) || (strcmp(builtinname,"/") == 0) || (strcmp(builtinname,"validate") == 0) || (strcmp(builtinname,"export") == 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int printconditional(WJElement proto, WJElement schema, WJElement face, int depth)
{
  WJElement option = NULL;
  char * facename = NULL;
  if (face)
    facename = face->name;
  else
    facename = NULL;

  if (WJEGet(optionlist(schema, facename), "if", NULL))
  {
    if (WJESchemaValidate(WJEGet(optionlist(schema, face->name), "if", NULL), protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      while (option = _WJEObject(WJEGet(optionlist(schema, face->name), "then", NULL), "properties[]", WJE_GET, &option))
      {
        if (!WJEBool(option, "hidden", WJE_GET, FALSE))
        {
          for (int i = depth; i > 0; i--)
          {
            printf("%s.", parentname(proto, i));
          }
          if (WJEGet(proto, "schema.patternProperties", NULL))
            printf("%s.", elementname(proto,face));
          printf("%s = ", option->name);
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
    else
    {
      while (option = _WJEObject(WJEGet(optionlist(schema, face->name), "else", NULL), "properties[]", WJE_GET, &option))
      {
        if (!WJEBool(option, "hidden", WJE_GET, FALSE))
        {
          for (int i = depth; i > 0; i--)
          {
            printf("%s.", parentname(proto, i));
          }
          if (WJEGet(proto, "schema.patternProperties", NULL))
            printf("%s.", elementname(proto,face));
          printf("%s = ", option->name);
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
      printconditional(proto, WJEGet(optionlist(schema, face->name), "else", NULL),face, depth);
    }
  }
}

int printoption(WJElement proto, WJElement face, int depth)
{
  WJElement option = NULL;
  WJElement schema = WJEGet(proto, "schema", NULL);
  char * facename = NULL;
  if (face)
    facename = face->name;
  else
    facename = NULL;
  while (option = _WJEObject(optionlist(schema, facename), "properties[]", WJE_GET, &option)) {
    if (!WJEBool(option, "hidden", WJE_GET, FALSE))
    {
      for (int i = depth; i > 0; i--)
      {
        printf("%s.", parentname(proto, i));
      }
      if (WJEGet(proto, "schema.patternProperties", NULL))
        printf("%s.", elementname(proto,face));
      printf("%s = ", option->name);

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
  printconditional(proto, schema, face, depth);
  return 0;
}

int printoption2(WJElement proto, int depth)
{
  WJElement face = NULL;
  while (face = _WJEObject(proto, "data[]", WJE_GET, &face)) {
    printoption(proto,face, depth);
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
          printoption(proto,WJEGet(proto, "data", NULL),depth);
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
    printoption(protojson,protoface,protodepth);
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

int exportconditional(WJElement proto, WJElement schema, WJElement face, int depth)
{
  WJElement option = NULL;
  char * facename = NULL;
  if (face)
    facename = face->name;
  else
    facename = NULL;

  if (WJEGet(optionlist(schema, facename), "if", NULL))
  {
    if (WJESchemaValidate(WJEGet(optionlist(schema, face->name), "if", NULL), protoface, schema_errorq, schema_load, schema_free, "%s"))
    {
      while (option = _WJEObject(WJEGet(optionlist(schema, face->name), "then", NULL), "properties[]", WJE_GET, &option))
      {
        if (!WJEBool(option, "hidden", WJE_GET, FALSE) && WJEGet(face, option->name, NULL))
        {
          for (int i = depth; i > 0; i--)
          {
            printf("%s ", parentname(proto, i));
          }
          if (WJEGet(proto, "schema.patternProperties", NULL))
            printf("%s ", elementname(proto,face));
          printf("%s ", option->name);
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
    else
    {
      while (option = _WJEObject(WJEGet(optionlist(schema, face->name), "else", NULL), "properties[]", WJE_GET, &option))
      {
        if (!WJEBool(option, "hidden", WJE_GET, FALSE) && WJEGet(face, option->name, NULL))
        {
          for (int i = depth; i > 0; i--)
          {
            printf("%s ", parentname(proto, i));
          }
          if (WJEGet(proto, "schema.patternProperties", NULL))
            printf("%s ", elementname(proto,face));
          printf("%s ", option->name);
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
      exportconditional(proto, WJEGet(optionlist(schema, face->name), "else", NULL),face, depth);
    }
  }
}

int exportoption(WJElement proto, WJElement face, int depth)
{
  WJElement option = NULL;
  WJElement schema = WJEGet(proto, "schema", NULL);
  char * facename = NULL;
  if (face)
    facename = face->name;
  else
    facename = NULL;
  while (option = _WJEObject(optionlist(schema, facename), "properties[]", WJE_GET, &option)) {
    if (!WJEBool(option, "hidden", WJE_GET, FALSE) && WJEGet(face, option->name, NULL))
    {
      for (int i = depth; i > 0; i--)
      {
        printf("%s ", parentname(proto, i));
      }
      if (WJEGet(proto, "schema.patternProperties", NULL))
        printf("%s ", elementname(proto,face));
      printf("%s ", option->name);

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
  exportconditional(proto, schema, face, depth);
  return 0;
}

int exportoption2(WJElement proto, int depth)
{
  WJElement face = NULL;
  while (face = _WJEObject(proto, "data[]", WJE_GET, &face)) {
    exportoption(proto,face, depth);
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
          exportoption(proto,WJEObject(proto, "data", WJE_GET),depth);
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
    exportoption(protojson,protoface,protodepth);
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
    if (WJESchemaValidate(WJEGet(protojson,"schema",NULL), WJEGet(protojson,"data",NULL), schema_error, schema_load, schema_free, "%s"))
    {
      puts("schema valid");
    }
    else
    {
      puts("schema invalid");
    }
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
    return path_root();
  }
  else if (strcmp(argv[0],"..") == 0)
  {
    return path_up();
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
