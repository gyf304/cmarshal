#ifndef _JSON_STRUCTS_H
#define _JSON_STRUCTS_H

#include <stdbool.h>

/* Global Configuration
cmarshal:`{
	"cJSONInclude": "./cJSON.h",
	"unmarshalerHeaderFile": "./jsonstructs.unmarshaler.h",
	"unmarshalerImplFile": "./jsonstructs.unmarshaler.c"
}`
*/;

/* 
 * CMarshalConfig is a global configuration on how the code generator
 * should behave. An example is shown above.
 *
 * cmarshal:`true`
 */
typedef struct {
	char *cJSONInclude;

	char *marshalerPrefix;
	char *unmarshalerPrefix;

	char *marshalerHeaderFile;
	char *marshalerImplFile;
	char *unmarshalerHeaderFile;
	char *unmarshalerImplFile;
} CMarshalConfig;

/* 
 * CMarshalTypeAnnotation is an annotation on a type definition.
 * This controls whether a public marshaler / unmarshaler will be
 * generated for the specified type.
 * You can also use `true` as a shorthand for enabling both.
 * See below for an example.
 *
 * cmarshal:`true`
 */
typedef struct {
	bool marshal;   /* enable marshaling */
	bool unmarshal; /* enable unmarshaling */
} CMarshalTypeAnnotation;

/* 
 * CMarshalMemberAnnotation is an annotation on a struct / union
 * member / field.
 * Use ignore to ignore a field, use key to rename a field,
 * or use length to specify a pointer field is an array and its
 * length field in the struct.
 *
 * cmarshal:`true`
 */
typedef struct {
	bool ignore;    /* ignore a field */
	char *key;      /* set an alternative key name */
	char *length;   /* set the length field for a member in struct */
} CMarshalMemberAnnotation;

#ifndef CMARSHAL_GENERATE
#include "./jsonstructs.unmarshaler.h"
#endif

#endif
