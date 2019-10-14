#include <readline/readline.h>
#include <readline/history.h>

char **character_name_completion(const char *, int, int);
char *character_name_generator(const char *, int);
int zc_completion(int count, int key);

extern enum domains rl_domain;

