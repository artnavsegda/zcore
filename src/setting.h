#include <wjelement.h>

int listsettings(void);
int issetting(char * optionname);
int rl_issetting(char * optionname);
int setting(int argc, char *argv[]);
int rl_setting(int argc, char *argv[]);
char * settingvalues(const char * text, int len, int state);
char * settinghelp(const char * commandname, WJElement proto, WJElement face);

char * conoptionvalues(const char * text, int len);
char * cuesettingvalues(const char * text, int len, int state);
char * optionvalue(char * commandname, WJElement proto, WJElement face);
WJElement conditionoption(WJElement schema, WJElement face, char * optionname);
int option_set_value(WJElement parameter, char * parametername, char * value);
WJElement optionlist(WJElement schema, char * protoname);
