//#include <wjelement.h>
#include <json-c/json.h>

// char *strmbtok ( char *input, char *delimit, char *openblock, char *closeblock);
// WJElement getelementbynameprop(WJElement container, char * text);
json_object * getelementbynameprop(json_object * container, char * text);
// char * cutquot(char * stringtocut);
int arrlength(char **array);
int parse(char * stringtoparse, char **tokarr);
// int ifacefound(char * ifacetosearch);
// int commandfound(char * commandtosearch);
// char * formatcommand(char * command);
// int forkwaitexec(char * command, int argc, char *argv[], char *envp[]);
// int streamfromcommand(char * command, char *argv[], char *envp[], WJElement jsonparent);
int my_popen_read (char * command, char *argv[], char *envp[]);
FILE * my_popen_write (char * command, char *argv[], char *envp[]);
// int streamintocommand(char * command, char *argv[], char *envp[], char * stream);
int arguments(json_object * argarray, const char *argv[]);
// int argcat(int argc, char *argout[], char *argin[]);
// void fillenv(WJElement proto, WJElement face);
// WJElement optionlist(WJElement schema);
//
