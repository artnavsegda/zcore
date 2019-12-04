int listcommands(void);
int iscommand(char * commandname);
int command(int argc, char *argv[]);
int rl_iscommand(char * commandname);
char * commandvalues(const char * text, int len);
int rl_command(int argc, char *argv[]);
char * cuecommandvalues(const char * text, int len, int state);
char * cuecuecommandvalues(const char * text, int len, int state);
char * commandhelp(const char * commandname, WJElement proto);
