#ifndef _DEMO_H
#define _DEMO_H

/*
cmarshal:`{
	"cJSONInclude": "./cJSON.h",
	"unmarshalerHeaderFile": "./demo.unmarshaler.h",
	"unmarshalerImplFile": "./demo.unmarshaler.c",
	"marshalerHeaderFile": "./demo.marshaler.h",
	"marshalerImplFile": "./demo.marshaler.c"
}`
*/;

/* cmarshal:`true` */
typedef struct {
	int hello;
	char *world;
	int dontcare; /* cmarshal:`{"ignore": true}` */
} MyStruct;
#endif
