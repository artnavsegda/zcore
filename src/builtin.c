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

extern int protodepth;
extern WJElement protojson;
extern WJElement protoface;

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
    break;
  }
  return 0;
}

int path_root()
{
  domain = PROTO;
  protodepth = 0;
  protojson = root;
}

int commandlist()
{
  puts("tokens:");
  listbuiltins();
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

void listbuiltins()
{
  puts("?");
  puts("..");
  puts("/");
  puts("show");
  puts("acquire");
  puts("validate");
}

int isbuiltin(char * builtinname)
{
  if ((strcmp(builtinname,"?") == 0) || (strcmp(builtinname,"..") == 0) || (strcmp(builtinname,"show") == 0) || (strcmp(builtinname,"acquire") == 0) || (strcmp(builtinname,"/") == 0) || (strcmp(builtinname,"validate") == 0))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int printoption(WJElement proto, WJElement face, int depth)
{
  WJElement option = NULL;
  while (option = _WJEObject(optionlist(proto), "properties[]", WJE_GET, &option)) {
    if (!WJEBool(option, "hidden", WJE_GET, FALSE))
    {
      for (int i = depth; i > 0; i--)
      {
        printf("%s.", parentname(proto, i));
      }
      if (WJEGet(proto, "schema.patternProperties", NULL))
        printf("%s.", elementname(proto,face));
      printf("%s = ", option->name);

      if (WJEGet(face,option->name,NULL))
      {
      if (strcmp(WJEString(option,"type", WJE_GET, NULL),"string") == 0)
        printf("%s", WJEString(face, option->name, WJE_GET, "None"));
      else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"number") == 0)
        printf("%d", WJEInt32(face,option->name,WJE_GET,-1));
      else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"boolean") == 0)
      {
        if (WJEBool(face,option->name,WJE_GET,-1) == TRUE)
          printf("True");
        else if (WJEBool(face,option->name,WJE_GET,-1) == FALSE)
          printf("False");
        else
          printf("None");
      }
      else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"array") == 0)
      {
        WJElement array = NULL;
        char * entity = NULL;
        while (entity = WJEStringF(face, WJE_GET, &array, NULL, "%s[]", option->name))
        {
          printf("%s ", entity);
        }
      }
      }
      else
        printf("None");
      puts("");
    }
  }
}

int printoption2(WJElement proto, int depth)
{
  WJElement face = NULL;
  while (face = _WJEObject(proto, "data[]", WJE_GET, &face)) {
    printoption(proto,face, depth);
  }
}

int printoption3(WJElement protoinput, int depth)
{
  WJElement proto = NULL;
  depth++;
  while ((proto = _WJEObject(protoinput, "[]", WJE_GET, &proto))) {
    if (WJEGet(proto, "schema", NULL))
    {
      if (WJEGet(proto, "schema.patternProperties", NULL))
      {
        printoption2(proto, depth);
      }
      else if (WJEGet(proto, "schema.properties", NULL))
      {
        printoption(proto,WJEObject(proto, "data", WJE_GET),depth);
      }
    }
    else
    {
      printoption3(proto, depth);
    }
  }
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
}

int builtin_acquire(int argc, char *argv[])
{
  acquire(WJEObject(root, argv[1], WJE_GET));
}

static void schema_error(void *client, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

static WJElement schema_load(const char *name, void *client,
							 const char *file, const int line) {
	char *format;
	char *path;
	FILE *schemafile;
	WJReader readschema;
	WJElement schema;

	schema = NULL;
	if(client && name) {
		format = (char *)client;
		path = malloc(strlen(format) + strlen(name));
		sprintf(path, format, name);

		if ((schemafile = fopen(path, "r"))) {
			if((readschema = WJROpenFILEDocument(schemafile, NULL, 0))) {
				schema = WJEOpenDocument(readschema, NULL, NULL, NULL);
			} else {
				fprintf(stderr, "json document failed to open: '%s'\n", path);
			}
			fclose(schemafile);
		} else {
			fprintf(stderr, "json file not found: '%s'\n", path);
		}
		free(path);
	}
  WJEDump(schema);
	return schema;
}

static void schema_free(WJElement schema, void *client) {
	WJECloseDocument(schema);
	return;
}

int builtin_validate(int argc, char *argv[])
{
  if (domain == FACE)
  {
    puts("schema:");
    WJEDump(WJEGet(protojson,"schema",NULL));
    puts("data:");
    WJEDump(WJEGet(protojson,"data",NULL));
    if (WJESchemaValidate(WJEGet(protojson,"schema",NULL), WJEGet(protojson,"data",NULL), schema_error, schema_load, schema_free, NULL))
    {
      puts("schema valid");
    }
    else
    {
      puts("schema invalid");
    }
  }
}

int builtin(int argc, char *argv[])
{
  if (argv[0][0]=='?')
  {
    return commandlist();
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
}

char *builtincommands[] = {"?","..","show","acquire","validate","/",NULL};

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
