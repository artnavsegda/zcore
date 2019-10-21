#define CONFIGPATH "/etc/zenith/zenith.json"
#define MAXPATH 255
#define DEFAULTSCHEMAPATH "/etc/zenith/schema"
#define SCHEMAEXTENSION ".schema.json"

struct configstruct_t
{
  char schemapath[MAXPATH];
};

extern struct configstruct_t config;

int readconfig(void);

