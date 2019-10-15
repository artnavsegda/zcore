#include <readline/readline.h>
#include <readline/history.h>
#include "domain.h"

extern enum domains rl_domain;

struct complement
{
  char * command;
  enum domains domain;
  char * description;
};

struct complestruct
{
  char * locode;
  int complecount;
  struct complement complelist[];
};

char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);
int zc_completion(int count, int key);

