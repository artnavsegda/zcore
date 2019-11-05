#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/wait.h>
#include <json-c/json.h>
#include "utils.h"
#include "option.h"


char *strmbtok ( char *input, char *delimit, char *openblock, char *closeblock) {
    static char *token = NULL;
    char *lead = NULL;
    char *block = NULL;
    int iBlock = 0;
    int iBlockIndex = 0;

    if ( input != NULL) {
        token = input;
        lead = input;
    }
    else {
        lead = token;
        if ( *token == '\0') {
            lead = NULL;
        }
    }

    while ( *token != '\0') {
        if ( iBlock) {
            if ( closeblock[iBlockIndex] == *token) {
                iBlock = 0;
            }
            token++;
            continue;
        }
        if ( ( block = strchr ( openblock, *token)) != NULL) {
            iBlock = 1;
            iBlockIndex = block - openblock;
            token++;
            continue;
        }
        if ( strchr ( delimit, *token) != NULL) {
            *token = '\0';
            token++;
            break;
        }
        token++;
    }
    return lead;
}

json_object * getelementbynameprop(json_object * container, char * text)
{
  json_object * entity = NULL;
  json_object * namesake = NULL;
  if (!json_pointer_get(container, "/schema/namesake", &namesake))
  {
  }
  else
  {
    if (!json_pointer_getf(container, &entity, "/data/%s", text))
    {
      return entity;
    }
    else
    {
      return NULL;
    }
  }
  return NULL;
}

// char * elementname(json_object * proto, json_object * element)
// {
//   char * namesake = WJEString(proto, "schema.namesake", WJE_GET, NULL);
//   if (namesake)
//   {
//     return WJEString(element, namesake, WJE_GET, "");
//   }
//   else
//   {
//     return element->name;
//   }
// }

char * cutquot(char * stringtocut)
{
  stringtocut[strlen(stringtocut)-1] = '\0';
  return stringtocut+1;
}

int arrlength(char **array)
{
  int length = 0;
  while (array[length] != NULL)
    length++;
  return length;
}

int parse(char * stringtoparse, char **tokarr)
{
  if (strlen(stringtoparse) == 0)
  {
    tokarr[0] = NULL;
    return 0;
  }

  char acOpen[]  = {"\"[<{"};
  char acClose[] = {"\"]>}"};
  int i = 0;
  tokarr[i] = stringtoparse;
  while ((tokarr[i] = strmbtok(tokarr[i], " ", acOpen, acClose)) != NULL)
    tokarr[++i] = NULL;
  return i;
}

int streamintocommand(char * command, char *argv[], char *envp[], char * stream)
{
  FILE *jsonstream = my_popen_write(command, argv, envp);
  if (jsonstream == NULL)
  {
    puts("streamintocommand handle error");
    return 1;
  }
  fwrite(stream,strlen(stream),1,jsonstream);
  pclose(jsonstream);
  return 0;
}

int forkwaitexec(char * command, int argc, char *argv[], char *envp[])
{
  pid_t pid = fork();
  int status;
  if (pid == 0)
  {
    execve(command,argv,envp);
    exit(0);
  }
  else
  {
    waitpid(pid, &status, 0);
  }
  return 0;
}

int streamfromcommand(char * command, char *argv[], char *envp[], json_object * jsonparent)
{
  int jsonstream = my_popen_read(command, argv, envp);
  if (jsonstream == -1)
  {
    puts("handle error");
    return 1;
  }
  json_object * readjson = NULL;
  if ((readjson = json_object_from_fd(jsonstream)) == NULL) {
    puts("json failed to open");
    return 1;
  }
  json_object_object_add(jsonparent, "data", readjson);
  return 0;
}

int my_popen_read (char * command, char *argv[], char *envp[])
{
    int fd[2];
    int read_fd, write_fd;
    int pid;
    pipe(fd);
    read_fd = fd[0];
    write_fd = fd[1];
    pid = fork();
    if (pid == 0) {
        close(read_fd);
        dup2(write_fd,1);
        close(write_fd);
        execve(command,argv,envp);
        return NULL;
    } else {
        close(write_fd);
        return read_fd;
    }
}

FILE * my_popen_write (char * command, char *argv[], char *envp[])
{
    int fd[2];
    int read_fd, write_fd;
    int pid;
    pipe(fd);
    read_fd = fd[0];
    write_fd = fd[1];
    pid = fork();
    if (pid == 0) {
        close(write_fd);
        dup2(read_fd,0);
        close(read_fd);
        execve(command,argv,envp);
        return NULL;
    } else {
        close(read_fd);
        return fdopen(write_fd, "w");
    }
}

int arguments(json_object * argarray, char *argv[])
{
  int i = 0;

  while (argv[i] = (char *)json_object_get_string(json_object_array_get_idx(argarray,i)))
  {
    i++;
  }

  return i;
}

int argcat(int argc, char *argout[], char *argin[])
{
  int i = 0;
  while (argout[argc] = argin[i])
  {
    i++;
    argc++;
  }
  return argc;
}

json_object * optionlist(json_object * schema)
{
  json_object * elementlist = NULL;
  json_object * patternprop = NULL;
  if(json_object_object_get_ex(schema, "schema", &elementlist))
  {
    if(json_object_object_get_ex(elementlist, "properties", NULL))
    {
      return elementlist;
    }
    else if (json_object_object_get_ex(elementlist, "patternProperties", &patternprop))
    {
      json_object_object_foreach(patternprop, key, val)
      {
        return val;
      }
    }
  }
  return 0;
}

// void fillenv(WJElement proto, WJElement face)
// {
//   int i = 0;
//   char stringparam[100] = "";
//   WJElement option = NULL;
//   while (option = _WJEObject(optionlist(proto), "properties[]", WJE_GET, &option)) {
//     if (WJEGet(face,option->name,NULL))
//       {
//         if (strcmp(WJEString(option,"type", WJE_GET, NULL),"string") == 0)
//         {
//           setenv(option->name,WJEString(face, option->name, WJE_GET, "None"),1);
//         }
//         else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"number") == 0)
//         {
//           sprintf(stringparam,"%d",WJEInt32(face,option->name,WJE_GET,-1),1);
//           setenv(option->name,stringparam,1);
//         }
//         else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"boolean") == 0)
//         {
//           if (WJEBool(face,option->name,WJE_GET,-1))
//             setenv(option->name,"True",1);
//           else
//             setenv(option->name,"False",1);
//         }
//       }
//   }
// }

int forkexec(char * command, int argc, char *argv[], char *envp[])
{
  pid_t pid = fork();
  int status;
  if (pid == 0)
  {
    execve(command,argv,envp);
    exit(0);
  }
  return 0;
}
