#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include "builtin.h"
#include "interpreter.h"
#include "proto.h"
// #include "zcore.h"
#include "face.h"
#include "command.h"
#include "option.h"
#include "prompt.h"
#include "acquire.h"
#include "utils.h"

extern int protodepth;
extern json_object * protojson;
extern json_object * protoface;
extern json_object * root;

path_t root_path = { .element = root, .name = "" };

path_t * path = &root_path;
path_t * rl_path = &root_path;

int path_up()
{
  json_object * schema = NULL;
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
        path = path->parent;
        protojson = path->element;
      }
    break;
     case FACE:
       domain = PROTO;
       protodepth--;
       path = path->parent;
       protojson = path->element;
     break;
     case OPTION:
       json_object_object_get_ex(protojson, "schema", &schema);
       if (json_object_object_get_ex(schema, "patternProperties", NULL))
       {
         domain = FACE;
       }
       else if (json_object_object_get_ex(schema, "properties", NULL))
       {
         domain = PROTO;
         protodepth--;
         path = path->parent;
         protojson = path->element;
       }
//       if (strcmp(WJEString(protojson,"schema.type",WJE_GET,"unknown"),"array") == 0)
//       {
//         domain = FACE;
//       }
//       else if (strcmp(WJEString(protojson,"schema.type",WJE_GET,"unknown"),"object") == 0)
//       {
//         domain = PROTO;
//         protodepth--;
//         protojson = protojson->parent;
//       }
     break;
  }
  return 0;
}

int path_root()
{
  domain = PROTO;
  protodepth = 0;
  protojson = root;
  path = &root_path;
  return 0;
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
}

int isbuiltin(char * builtinname)
{
  if ((strcmp(builtinname,"?") == 0) || (strcmp(builtinname,"..") == 0) || (strcmp(builtinname,"show") == 0) || (strcmp(builtinname,"acquire") == 0) || (strcmp(builtinname,"/")) == 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int printoption(json_object * proto, json_object * face, int depth)
{
  json_object * options = NULL;
  json_object * hidden = NULL;
  json_object * protovalue = NULL;

  if (json_object_object_get_ex(optionlist(proto), "properties", &options))
  {
    json_object_object_foreach(options,key, val)
    {
      json_object_object_get_ex(val, "hidden", &hidden);
      if (!json_object_get_boolean(hidden))
      {
        for (int i = depth; i > 0; i--)
        {
          printf("%s.", parentname(path, i));
        }
        if (!json_pointer_get(proto, "/schema/patternProperties", NULL))
        {
          printf("%s.", elementname(proto,face));
        }
        printf("%s = ",key);
        if (json_object_object_get_ex(face,key,&protovalue))
        {
          puts(json_object_to_json_string_ext(protovalue, JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOSLASHESCAPE));
        }
        else
        {
          puts("None");
        }
      }
    }
  }
//   WJElement option = NULL;
//   while (option = _WJEObject(optionlist(proto), "properties[]", WJE_GET, &option)) {
//     if (!WJEBool(option, "hidden", WJE_GET, FALSE))
//     {
//       for (int i = depth; i > 0; i--)
//       {
//         printf("%s.", parentname(proto, i));
//       }
//       if (WJEGet(proto, "schema.patternProperties", NULL))
//         printf("%s.", elementname(proto,face));
//       printf("%s = ", option->name);
//
//       if (WJEGet(face,option->name,NULL))
//       {
//         if (strcmp(WJEString(option,"type", WJE_GET, NULL),"string") == 0)
//           printf("%s", WJEString(face, option->name, WJE_GET, "None"));
//         else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"number") == 0)
//           printf("%d", WJEInt32(face,option->name,WJE_GET,-1));
//         else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"boolean") == 0)
//         {
//           if (WJEBool(face,option->name,WJE_GET,-1) == TRUE)
//             printf("True");
//           else if (WJEBool(face,option->name,WJE_GET,-1) == FALSE)
//             printf("False");
//           else
//             printf("None");
//         }
//         else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"array") == 0)
//         {
//           WJElement array = NULL;
//           if (strcmp(WJEString(option,"items.type", WJE_GET, NULL),"string") == 0){
//             char * entity = NULL;
//             while (entity = WJEStringF(face, WJE_GET, &array, NULL, "%s[]", option->name))
//             {
//               printf("%s ", entity);
//             }
//           }
//           else if (strcmp(WJEString(option,"items.type", WJE_GET, NULL),"number") == 0){
//             int number = 0;
//             while (number = WJEInt32F(face, WJE_GET, &array, 0, "%s[]", option->name))
//               printf("%d ", number);
//           }
//         }
//       }
//       else
//         printf("None");
//       puts("");
//     }
// }
  return 0;
}

int printoption2(json_object * proto, int depth)
{
  json_object * face = NULL;
  json_object_object_get_ex(proto, "data", &face);

  json_object_object_foreach(face, key,val)
  {
    printoption(proto,face, depth);
  }

  // while (face = _WJEObject(proto, "data[]", WJE_GET, &face)) {
  //   printoption(proto,face, depth);
  // }
  return 0;
}

// int printoption3(WJElement protoinput, int depth)
// {
//   WJElement proto = NULL;
//   depth++;
//   while ((proto = _WJEObject(protoinput, "[]", WJE_GET, &proto))) {
//     if (WJEGet(proto, "schema", NULL))
//     {
//       if (strcmp(WJEString(proto,"schema.type",WJE_GET,"unknown"),"array") == 0)
//       {
//         printoption2(proto, depth);
//       }
//       else if (strcmp(WJEString(proto,"schema.type",WJE_GET,"unknown"),"object") == 0)
//       {
//         printoption(proto,WJEObject(proto, "data", WJE_GET),depth);
//       }
//     }
//     else
//     {
//       printoption3(proto, depth);
//     }
//   }
// }

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
    // printoption3(protojson,protodepth);
  }
  else
    puts("Not implemented");
  return 1;
}

// int builtin_acquire(int argc, char *argv[])
// {
//   acquire(WJEObject(root, argv[1], WJE_GET));
// }

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
  // else if (strcmp(argv[0],"acquire") == 0)
  // {
  //   return builtin_acquire(argc,argv);
  // }
  return 0;
}

char *builtincommands[] = {"?","..","show","acquire","/",NULL};

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
