#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "utils.h"
#include "option.h"
#include "command.h"
#include "setting.h"
#include "config.h"
#include "validate.h"

extern WJElement doc, schema;
extern WJElement protoface;
extern WJElement protojson;
extern WJElement protoschema;
extern WJElement root;
extern int optiondepth;

WJElement getelementbynameprop(WJElement container, char * text)
{
  WJElement entity = NULL;
  char * namesake = WJEString(container, "schema.namesake", WJE_GET, NULL);
  while (entity = _WJEObject(container, "data[]", WJE_GET, &entity)) {
    if (namesake)
    {
      if (strcmp(WJEString(entity, namesake, WJE_GET, ""), text) == 0) {
        return entity;
      }
    }
    else
    {
      if (strcmp(entity->name, text) == 0) {
        return entity;
      }
    }
  }
  return NULL;
}

char * elementname(WJElement proto, WJElement element)
{
  char * namesake = WJEString(proto, "schema.namesake", WJE_GET, NULL);
  if (namesake)
  {
    return WJEString(element, namesake, WJE_GET, "");
  }
  else
  {
    return element->name;
  }
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
  //while ((tokarr[i] = strmbtok(tokarr[i], " ", acOpen, acClose)) != NULL)
  while ((tokarr[i] = strtok(tokarr[i], " \n")) != NULL)
    tokarr[++i] = NULL;
  return i;
}

int streamintocommand(char * command, char *argv[], char *envp[], char * stream)
{
  int forkpid, status;
  FILE *jsonstream = my_popen_write(command, argv, envp, &forkpid);
  if (jsonstream == NULL)
  {
    puts("streamintocommand handle error");
    return 1;
  }
  fwrite(stream,strlen(stream),1,jsonstream);
  fflush(jsonstream);
  fclose(jsonstream);
  waitpid(forkpid, &status, 0);
  return WEXITSTATUS(status);
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
    return WEXITSTATUS(status);
  }
}

int forkexec(char * command, int argc, char *argv[], char *envp[])
{
  pid_t pid = fork();
  int status;
  if (pid == 0)
  {
    execve(command,argv,envp);
    perror("ERROR");
    exit(0);
  }
  return 0;
}

WJElement streamfromcommand(char * command, char *argv[], char *envp[])
{
  int forkpid, status;
  FILE *jsonstream = my_popen_read(command, argv, envp, &forkpid);
  if (jsonstream == NULL)
  {
    puts("handle error");
    return NULL;
  }

  WJReader readjson = WJROpenFILEDocument(jsonstream, NULL, 0);
  if (readjson == NULL) {
    puts("json failed to open");
    return NULL;
  }
  WJElement jsondata = WJEOpenDocument(readjson, NULL, NULL, NULL);
  //WJEAttach(jsonparent,jsondata);

  puts("receved data from stream");
  WJEDump(jsondata);

  fclose(jsonstream);
  waitpid(forkpid, &status, 0);

  printf("return code %d\n",status);

  if (status == 0)
    return jsondata;
  else
    return NULL;
}

FILE * my_popen_read (char * command, char *argv[], char *envp[], int * pid)
{
    int fd[2];
    int read_fd, write_fd;
    //int pid;
    pipe(fd);
    read_fd = fd[0];
    write_fd = fd[1];
    *pid = fork();
    if (*pid == 0) {
        close(read_fd);
        dup2(write_fd,1);
        close(write_fd);
        execve(command,argv,envp);
        return NULL;
    } else {
        close(write_fd);
        return fdopen(read_fd, "r");
    }
}

FILE * my_popen_write (char * command, char *argv[], char *envp[], int * pid)
{
    int fd[2];
    int read_fd, write_fd;
    //int pid;
    pipe(fd);
    read_fd = fd[0];
    write_fd = fd[1];
    *pid = fork();
    if (*pid == 0) {
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

int arguments(WJElement argarray, char *argv[])
{
  int i = 1;
  WJElement argstring = NULL;

  while (argv[i] = _WJEString(argarray, "[]", WJE_GET, &argstring, NULL))
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

WJElement optionsdepth(WJElement schema)
{
  if (optiondepth > 0)
  {
    puts("depth");
    WJEDump(schema);
    return WJEGetF(schema, NULL, "properties.%s",protoface->name);
  }
  else
    return schema;
}

void fillenv(WJElement proto, WJElement face)
{
  int i = 0;
  char stringparam[100] = "";
  WJElement option = NULL;
  while (option = _WJEObject(optionlist(proto, face->name), "properties[]", WJE_GET, &option)) {
    if (WJEGet(face,option->name,NULL))
      {
        if (strcmp(WJEString(option,"type", WJE_GET, NULL),"string") == 0)
        {
          setenv(option->name,WJEString(face, option->name, WJE_GET, "None"),1);
        }
        else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"number") == 0)
        {
          sprintf(stringparam,"%d",WJEInt32(face,option->name,WJE_GET,-1),1);
          setenv(option->name,stringparam,1);
        }
        else if (strcmp(WJEString(option,"type", WJE_GET, NULL),"boolean") == 0)
        {
          if (WJEBool(face,option->name,WJE_GET,-1))
            setenv(option->name,"True",1);
          else
            setenv(option->name,"False",1);
        }
      }
  }
}

int onset(char * parametername, WJElement tempproto, char * value)
{
  //onset
  if (WJEGet(protojson,"schema.onset.command", NULL))
  {
    struct stat filestat;
    char onsetcommand[MAXPATH] = "\0";
    strcpy(onsetcommand,WJEString(protojson, "schema.onset.command", WJE_GET, NULL));
    if (stat(onsetcommand,&filestat))
    {
      onsetcommand[0] = '\0';
      strcat(onsetcommand, config.scriptpath);
      strcat(onsetcommand, "/");
      strcat(onsetcommand, WJEString(protojson, "schema.onset.command", WJE_GET, NULL));
    }
    if (stat(onsetcommand,&filestat))
    {
      puts("onset script inaccessible");
    }
    else
    {
      char *args[100];
      args[0] = onsetcommand;
      char * optionstring = optionvalue(parametername, protoschema, protoface);

      int argsc = arguments(WJEArray(protojson, "schema.onset.args", WJE_GET),args);

      char combinedepth[1000]; // replace with asprintf && free

     if (optiondepth > 0)
     {
       args[argsc++] = protoface->parent->name;
       snprintf(combinedepth,1000,"%s.%s", protoface->name, parametername);
       args[argsc++] = combinedepth;
     }
     else
     {
        args[argsc++] = protoface->name;
        args[argsc++] = parametername;
     }

     if (value[0] == '-')
      args[argsc++] = "";
     else
      args[argsc++] = value;
     args[argsc++] = NULL;

      if (WJEBool(protojson, "schema.onset.merge", WJE_GET, FALSE) == TRUE)
      {
        printf("merging %s %s %s %s %s %s\n", onsetcommand, args[1], args[2], args[3], args[4], args[5]);
        WJElement mergedata = streamfromcommand(onsetcommand,args,NULL);
        if (mergedata)
        {
          WJEMergeObjects(tempproto, mergedata, TRUE);
        }
        else
          puts("no data");
        WJEDump(tempproto);
      }
      else
      {
        forkwaitexec(onsetcommand,argsc,args,NULL);
      }
      free(optionstring);
    }
  }
}

int validate(WJElement temp, WJElement tempproto, char * parametername)
{
  WJElement tempschema = WJEObject(NULL, NULL, WJE_NEW);
  WJECopyDocument(tempschema, protoschema, NULL, NULL);

  //validate
  if (WJESchemaValidate(tempschema, tempproto, schema_error, schema_load, schema_free, "%s"))
  {
    char tempprotoname[100];
    strcpy(tempprotoname, protoface->name);
    WJECloseDocument(WJEGet(protojson,"data",NULL));

    if (WJEGet(protojson, "schema.patternProperties", NULL))
    {
      WJEAttach(protojson,tempproto);
      protoface = WJEGet(WJEGet(protojson,"data",NULL),tempprotoname,NULL);
      if (!protoface)
      {
        puts("FATAL ERROR");
        exit(0);
      }
    }
    else
    {
      WJEAttach(protojson,tempproto);
      protoface = WJEObject(protojson, "data", WJE_GET);
    }
    WJECloseDocument(tempschema);
    return 1;
  }
  else
  {
    puts("Schema validation failed, check output below for mismatches");
    WJECloseDocument(tempschema);
    WJECloseDocument(tempproto);
    return 0;
  }
}
