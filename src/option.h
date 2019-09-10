int listoptions(void);
int isoption(char * optionname);
int rl_isoption(char * optionname);
int option(int argc, char *argv[]);
int rl_option(int argc, char *argv[]);
char * optionvalues(const char * text, int len);
char * settingvalues(const char * text, int len, int state);

