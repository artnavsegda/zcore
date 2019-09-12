#include <wjelement.h>

char *strmbtok ( char *input, char *delimit, char *openblock, char *closeblock);
WJElement getelementbynameprop(WJElement container, char * text);
char * cutquot(char * stringtocut);
int arrlength(char **array);
int parse(char * stringtoparse, char **tokarr);
int ifacefound(char * ifacetosearch);
int streamintocommand(char * command, char * stream, char * argument);
int ifacefound(char * ifacetosearch);
int commandfound(char * commandtosearch);
char * formatcommand(char * command);
int forkwaitexec(char * command, int argc, char *argv[]);
int streamfromcommand(char * command, char * argument, WJElement jsonparent);

