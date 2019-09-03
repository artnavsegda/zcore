#define CONFIGPATH "/etc/zenith.json"
#define MAXPATH 255
#define DEFAULTSCHEMEPATH "/var/lib/zenith/scheme"

struct configstruct_t
{
  char schemepath[MAXPATH];
};

extern struct configstruct_t config;
