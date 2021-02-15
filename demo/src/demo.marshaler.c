/* auto-generated, do not edit */
#include "./demo.marshaler.h"
#include <string.h>
static cJSON* marshal_int(int* src);
static cJSON* marshal_char_p(char ** src);
static cJSON* marshal_MyStruct2_p(MyStruct2 ** src);
static cJSON* marshal_int_p(int ** src);
static cJSON* marshal_size_t(size_t* src);
static cJSON* marshal_MyStruct2(MyStruct2* src);
cJSON* marshal_MyStruct(MyStruct* src) 
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddItemToObject(obj, "hello", marshal_int(&src->hello));
	cJSON_AddItemToObject(obj, "world", marshal_char_p(&src->world));
	cJSON_AddItemToObject(obj, "nested", marshal_MyStruct2_p(&src->nested));
	{ /* field arr */
		cJSON* arr = cJSON_CreateArray();
		for (int i = 0; i < (int)(src->len); i++) {
			cJSON_AddItemToArray(arr, marshal_int(&src->arr[i]));
		}
		cJSON_AddItemToObject(obj, "arr", arr);
	}
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
static cJSON* marshal_MyStruct2_p(MyStruct2 ** src) 
{
	if (*src == NULL)
		return cJSON_CreateNull();
	return marshal_MyStruct2(*src);
}
static cJSON* marshal_int_p(int ** src) 
{
	if (*src == NULL)
		return cJSON_CreateNull();
	return marshal_int(*src);
}
static cJSON* marshal_size_t(size_t* src) 
{
	return marshal_unsigned_long(src);
}
static cJSON* marshal_MyStruct2(MyStruct2* src) 
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddItemToObject(obj, "a", marshal_int(&src->a));
	cJSON_AddItemToObject(obj, "b", marshal_int(&src->b));
	return obj;
}
cJSON* marshal_unsigned_long(unsigned long* src) 
{
	return cJSON_CreateNumber((double) *src);
}
