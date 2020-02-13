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
WJElement streamfromcommand(char * command, char *argv[], char *envp[]);
FILE * my_popen_read (char * command, char *argv[], char *envp[], int * pid);
FILE * my_popen_write (char * command, char *argv[], char *envp[], int * pid);
int streamintocommand(char * command, char *argv[], char *envp[], char * stream);
int arguments(WJElement argarray, char *argv[]);
int argcat(int argc, char *argout[], char *argin[]);
void fillenv(WJElement proto, WJElement face);
char * elementname(WJElement proto, WJElement element);
int forkexec(char * command, int argc, char *argv[], char *envp[]);
