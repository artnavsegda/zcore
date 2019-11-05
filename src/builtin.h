#include <json-c/json.h>

int commandlist();
void listbuiltins();
int isbuiltin(char * builtinname);
int builtin(int argc, char *argv[]);
char * builtinvalues(const char * text, int len);

#ifndef SOME_HEADER_GUARD_WITH_UNIQUE_NAME
#define SOME_HEADER_GUARD_WITH_UNIQUE_NAME

struct path_s
{
    json_object * element;
    struct path_s * parent;
    char * name;
};

typedef struct path_s path_t;

#endif
