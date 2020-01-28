#include <wjelement.h>

char * conoptionvalues(const char * text, int len);
char * settingvalues(const char * text, int len, int state);
char * cuesettingvalues(const char * text, int len, int state);
char * optionvalue(char * commandname, WJElement proto, WJElement face);
WJElement conditionoption(WJElement schema, WJElement face, char * optionname);
int option_set_value(WJElement parameter, char * parametername, char * value);
