#ifndef _JSON_STRUCTS_H
#define _JSON_STRUCTS_H

#include <stdbool.h>

/* Global Configuration
cmarshal:`{
	"unmarshalerHeaderFile": "./jsonstructs.gen.h",
	"unmarshalerImplFile": "./jsonstructs.gen.c"
}`
*/;

/* cmarshal:`true` */
typedef struct {
	char *cJSONInclude;

	char *marshalerPrefix;
	char *unmarshalerPrefix;

	char *marshalerHeaderFile;
	char *marshalerImplFile;
	char *unmarshalerHeaderFile;
	char *unmarshalerImplFile;
} CMarshalConfig;

/* cmarshal:`true` */
typedef struct {
	/* type annotations */
	bool enabled;   /* enable both marshaling and unmarshaling */
	bool marshal;   /* enable marshaling */
	bool unmarshal; /* enable unmarshaling */
} CMarshalTypeAnnotation;

/* cmarshal:`true` */
typedef struct {
	bool boolean;   /* forces the current field as boolean */
	bool required;  /* set a field as required */
	bool ignore;    /* ignore a field */
	char *key;      /* set an alternative key name */
	char *length;   /* set the length field for a member in struct */
} CMarshalMemberAnnotation;

#endif
