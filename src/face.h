#include <wjelement.h>

int isface(char * facename);
int rl_isface(char * facename);
int face(int argc, char *argv[]);
int rl_face(int argc, char *argv[]);
int listfaces(void);
char * facevalues(const char * text, int len);
char * facehelp(char * commandname, WJElement proto);
