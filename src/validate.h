void schema_free(WJElement schema, void *client);
WJElement schema_load(const char *name, void *client, const char *file, const int line);
void schema_error(void *client, const char *format, ...);
void schema_errorq(void *client, const char *format, ...);
