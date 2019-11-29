#include <wjelement.h>

void addglobalcommand(WJElement loadroot, char * commandname);
void listglobals(int argc, char *argv[]);
int isglobal(char * globalname);
int global(int argc, char *argv[]);
char * globalvalues(const char * text, int len);
