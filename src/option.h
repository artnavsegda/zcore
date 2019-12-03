#include <wjelement.h>

int listoptions(void);
int isoption(char * optionname);
int rl_isoption(char * optionname);
int option(int argc, char *argv[]);
int rl_option(int argc, char *argv[]);
char * optionvalues(const char * text, int len);
char * settingvalues(const char * text, int len, int state);
char * cuesettingvalues(const char * text, int len, int state);
char * optionhelp(const char * commandname, WJElement proto, WJElement face);
char * optionvalue(char * commandname, WJElement proto, WJElement face);
