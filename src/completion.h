#include <readline/readline.h>
#include <readline/history.h>
#include "domain.h"

extern enum domains rl_domain;

struct complement
{
  char * command;
  enum domains domain;
  char * description;
  char * value;
};

typedef struct complement cmpstr_t;

struct complestruct
{
  char * locode;
  int complecount;
  int maxlen;
  cmpstr_t **complelist;
};

typedef struct complestruct cmplist_t;

typedef cmpstr_t *callback_func_t (char *);

char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);
int zc_completion2(int count, int key);
int zc_completion3(int count, int key);
