#define CONFIGPATH "/etc/zenith/zenith.json"
#define MAXPATH 255
#define DEFAULTSCHEMAPATH "/etc/zenith/schema"
#define DEFAULTSCRIPTPATH "/etc/zenith/script"
#define DEFAULTJSONPATH "/etc/zenith/consts"
#define SCHEMAEXTENSION ".schema.json"

struct configstruct_t
{
  char schemapath[MAXPATH];
  char scriptpath[MAXPATH];
  char jsonpath[MAXPATH];
};

extern struct configstruct_t config;

int readconfig(char * configpath);
