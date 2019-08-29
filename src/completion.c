#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <wjelement.h>
#include <wjreader.h>

char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);

char * entityvalues(const char * text, int len)
{
  while (entity = _WJEObject(doc, "[]", WJE_GET, &entity)) {
    if (strncmp(WJEString(entity, "name", WJE_GET, ""), text, len) == 0) {
      return strdup(WJEString(entity, "name", WJE_GET, ""));
    }
  }
}

char * parametervalues(const char * text, int len)
{
  while (parameter = _WJEObject(schema, "items.properties[]", WJE_GET, &parameter)) {
    if (strncmp(parameter->name, text, len) == 0) {
      return strdup(parameter->name);
    }
  }
}

char * settingvalues(char * getparam, int state)
{
  if (!state)
  {
    entity = getelementbynameprop(doc,interface);
    char temp[100];
    sprintf(temp,"items.properties.%s",getparam);
    parameter = WJEObject(schema, temp, WJE_GET);
    char * testquote = WJEString(entity, parameter->name, WJE_GET, "");
    if (strchr(testquote,' '))
    {
      sprintf(temp,"\"%s\"", testquote);
      return strdup(temp);
    }
    else
      return strdup(WJEString(entity, parameter->name, WJE_GET, ""));
  }
  return NULL;
}

char * find_command(char **tokarr, int start)
{
  int numberoftokens = arrlength(tokarr);
  if (numberoftokens > 0)
  {
    if (start > 0)
      return tokarr[0];
  }
  return NULL;
}

char ** character_name_completion(const char *text, int start, int end)
{
    char *tokarr[100];
    char *inputline = strdup(rl_line_buffer);
    parse(inputline, tokarr);
    command = find_command(tokarr, start);
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, character_name_generator);
}

char * character_name_generator(const char *text, int state)
{
    static int list_index, len;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    switch (level)
    {
      case 0:
        if (command == NULL)
          return entityvalues(text,len);
        else
          return parametervalues(text,len);
      break;
      case 1:
        if (command == NULL)
          return parametervalues(text,len);
        else
          return settingvalues(command, state);
      break;
      case 2:
        return settingvalues(option, state);
      break;
    }

    return NULL;
}
