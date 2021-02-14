#include "./demo.marshaler.h"
#include <string.h>
static cJSON* marshal_int(int* src);
static cJSON* marshal_char_p(char ** src);
cJSON* marshal_MyStruct(MyStruct* src) 
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddItemToObject(obj, "hello", marshal_int(&src->hello));
	cJSON_AddItemToObject(obj, "world", marshal_char_p(&src->world));
	return obj;
}
static cJSON* marshal_int(int* src) 
{
	return cJSON_CreateNumber((double) *src);
}
static cJSON* marshal_char_p(char ** src) 
{
	if (*src == NULL)
		return cJSON_CreateNull();
	return cJSON_CreateString(*src);
}
