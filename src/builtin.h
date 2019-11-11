#include <wjelement.h>

int commandlist(int argc, char *argv[]);
void listbuiltins(int argc, char *argv[]);
int isbuiltin(char * builtinname);
int builtin(int argc, char *argv[]);
char * builtinvalues(const char * text, int len);
void schema_free(WJElement schema, void *client);
WJElement schema_load(const char *name, void *client, const char *file, const int line);
void schema_error(void *client, const char *format, ...);

