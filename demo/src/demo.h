#ifndef _DEMO_H
#define _DEMO_H

#include <stddef.h>

/*
cmarshal:`{
	"cJSONInclude": "./cJSON.h",
	"unmarshalerHeaderFile": "./demo.unmarshaler.h",
	"unmarshalerImplFile": "./demo.unmarshaler.c",
	"marshalerHeaderFile": "./demo.marshaler.h",
	"marshalerImplFile": "./demo.marshaler.c"
}`
*/;

typedef struct {
	int a;
	int b;
} MyStruct2;

/* cmarshal:`true` */
typedef struct {
	int hello;
	char *world;
	MyStruct2 *nested;
	int dontcare; /* cmarshal:`{"ignore": true}` */
	int *arr; /* cmarshal:`{"length": "len"}` */
	size_t len; /* cmarshal:`{"ignore": true}` */
} MyStruct;

#endif
