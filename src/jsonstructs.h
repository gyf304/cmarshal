#ifndef _JSON_STRUCTS_H
#define _JSON_STRUCTS_H

#include <stdbool.h>

/* Global Configuration
cmarshal:`{
	"unmarshalerHeaderFile": "./jsonstructs.unmarshaler.h",
	"unmarshalerImplFile": "./jsonstructs.unmarshaler.c"
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
	bool marshal;   /* enable marshaling */
	bool unmarshal; /* enable unmarshaling */
} CMarshalTypeAnnotation;

/* cmarshal:`true` */
typedef struct {
	bool ignore;    /* ignore a field */
	char *key;      /* set an alternative key name */
	char *length;   /* set the length field for a member in struct */
} CMarshalMemberAnnotation;

#endif
