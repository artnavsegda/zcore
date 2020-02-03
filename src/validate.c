#include <wjelement.h>
#include <string.h>

extern WJElement root;

void schema_error(void *client, const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

void schema_errorq(void *client, const char *format, ...) {
  return;
}

WJElement schema_load(const char *name, void *client,
							 const char *file, const int line) {
	char *format;
	char *path;
	FILE *schemafile;
	WJReader readschema;
	WJElement schema;

	schema = NULL;
	if(client && name) {
		format = (char *)client;
		path = malloc(strlen(format) + strlen(name));
		sprintf(path, format, name);

		if ((schemafile = fopen(path, "r"))) {
			if((readschema = WJROpenFILEDocument(schemafile, NULL, 0))) {
				schema = WJEOpenDocument(readschema, NULL, NULL, NULL);
			} else {
				fprintf(stderr, "json document failed to open: '%s'\n", path);
			}
			fclose(schemafile);
		} else {
			fprintf(stderr, "json file not found: '%s'\n", path);
		}
		free(path);
	}
  WJEDump(schema);
	return schema;
}

void schema_free(WJElement schema, void *client) {
	WJECloseDocument(schema);
	return;
}
