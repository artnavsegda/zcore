#include <wjelement.h>

char *strmbtok ( char *input, char *delimit, char *openblock, char *closeblock);
WJElement getelementbynameprop(WJElement container, char * text);
char * cutquot(char * stringtocut);
int arrlength(char **array);
int parse(char * stringtoparse, char **tokarr);