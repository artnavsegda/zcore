#include <wjelement.h>

char *strmbtok ( char *input, char *delimit, char *openblock, char *closeblock);
WJElement getelementbynameprop(WJElement container, char * text);
char * cutquot(char * stringtocut);
int arrlength(char **array);
int parse(char * stringtoparse, char **tokarr);
int ifacefound(char * ifacetosearch);
int commandfound(char * commandtosearch);
char * formatcommand(char * command);
int forkwaitexec(char * command, int argc, char *argv[], char *envp[]);
int streamfromcommand(char * command, char *argv[], char *envp[], WJElement jsonparent);
FILE * my_popen_read (char * command, char *argv[], char *envp[]);
FILE * my_popen_write (char * command, char *argv[], char *envp[]);
int streamintocommand(char * command, char *argv[], char *envp[], char * stream);
int arguments(WJElement argarray, char *argv[]);
int argcat(int argc, char *argout[], char *argin[]);

