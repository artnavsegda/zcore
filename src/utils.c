#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"

extern WJElement doc, schema;

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

WJElement getelementbynameprop(WJElement container, char * text)
{
  WJElement entity = NULL;
  while (entity = _WJEObject(container, "data[]", WJE_GET, &entity)) {
    if (strcmp(WJEString(entity, "name", WJE_GET, ""), text) == 0) {
      return entity;
    }
  }
  return NULL;
}

/*int ifacefound(char * ifacetosearch)
{
  if (getelementbynameprop(doc,ifacetosearch))
      return 1;
  else
      return 0;
}*/

/*int commandfound(char * commandtosearch)
{
  if (WJEObjectF(schema, WJE_GET, NULL, "commands.%s", commandtosearch))
    return 1;
  else
    return 0;
}*/

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

int streamintocommand(char * command, char *argv[], char * stream)
{
  //FILE *jsonstream = popen(command, "w");
  FILE *jsonstream = my_popen_write(command, argv);
  if (jsonstream == NULL)
  {
    puts("streamintocommand handle error");
    return 1;
  }
  fwrite(stream,strlen(stream),1,jsonstream);
  pclose(jsonstream);
}

/*char * formatcommand(char * command)
{
  return WJEStringF(schema, WJE_GET, NULL, "not found","commands.%s.command", command);
}*/

int forkwaitexec(char * command, int argc, char *argv[])
{
  pid_t pid = fork();
  int status;
  if (pid == 0)
  {
    execv(command,argv);
    exit(0);
  }
  else
  {
    waitpid(pid, &status, 0);
  }
}

int streamfromcommand(char * command, char *argv[], WJElement jsonparent)
{
  //FILE *jsonstream = popen(command, "r");
  FILE *jsonstream = my_popen_read(command, argv);
  if (jsonstream == NULL)
  {
    puts("handle error");
    return 1;
  }

  WJReader readjson = WJROpenFILEDocument(jsonstream, NULL, 0);
  if (readjson == NULL) {
    puts("json failed to open");
    return 1;
  }
  WJElement jsondata = WJEOpenDocument(readjson, NULL, NULL, NULL);
  WJEAttach(jsonparent,jsondata);

//  pclose(jsonstream);
}

FILE * my_popen_read (char * command, char *argv[])
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
        execv(command,argv);
        return NULL;
    } else {
        close(write_fd);
        return fdopen(read_fd, "r");
    }
}

FILE * my_popen_write (char * command, char *argv[])
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
        execv(command,argv);
        return NULL;
    } else {
        close(read_fd);
        return fdopen(write_fd, "w");
    }
}

char ** arguments(WJElement argarray, char *argv[])
{
  int i = 0;
  WJElement argstring = NULL;

  while (argv[i] = _WJEString(argarray, "[]", WJE_GET, &argstring, NULL))
  {
    i++;
  }

  return argv;
}

