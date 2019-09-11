#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>
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

int streamintocommand(char * command, char * stream, char * argument)
{
  char temp[100];
  sprintf(temp,command,argument);
  //FILE *jsonstream = popen(command, "w");
  FILE *jsonstream = popen(temp, "w");
  if (jsonstream == NULL)
  {
    puts("handle error");
    return 1;
  }
  fwrite(stream,strlen(stream),1,jsonstream); 
  pclose(jsonstream);
}

/*char * formatcommand(char * command)
{
  return WJEStringF(schema, WJE_GET, NULL, "not found","commands.%s.command", command);
}*/


