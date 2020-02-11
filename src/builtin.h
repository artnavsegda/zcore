#include <wjelement.h>

int commandlist(int argc, char *argv[]);
void listbuiltins(int argc, char *argv[]);
int isbuiltin(char * builtinname);
int rl_isbuiltin(char * builtinname);
int builtin(int argc, char *argv[]);
int rl_builtin(int argc, char *argv[]);
char * builtinvalues(const char * text, int len);
