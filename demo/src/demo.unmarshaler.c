#include "./demo.unmarshaler.h"
#include <string.h>
static enum unmarshal_status unmarshal_int(int* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
static enum unmarshal_status unmarshal_char_p(char ** dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
enum unmarshal_status unmarshal_MyStruct(MyStruct* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (!cJSON_IsObject(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	cJSON *item = NULL;
	enum unmarshal_status status = UNMARSHAL_OK;
	item = cJSON_GetObjectItemCaseSensitive(src, "hello");
	if (item != NULL) {
		status = unmarshal_int(&dst->hello, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "world");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->world, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	return UNMARSHAL_OK;
}
static enum unmarshal_status unmarshal_int(int* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (!cJSON_IsNumber(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	*dst = (int)cJSON_GetNumberValue(src);
	return UNMARSHAL_OK;
}
static enum unmarshal_status unmarshal_char_p(char ** dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (cJSON_IsNull(src)) {
		*dst = NULL;
		return UNMARSHAL_OK;
	}
	if (!cJSON_IsString(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	char *s = cJSON_GetStringValue(src);
	size_t len = strlen(s);
	if (allocator == NULL)
		return UNMARSHAL_ERROR_CANNOT_ALLOCATE;
	*dst = allocator(allocator_context, len+1);
	if (*dst == NULL)
		return UNMARSHAL_ERROR_CANNOT_ALLOCATE;
	memcpy(*dst, s, len+1);
	return UNMARSHAL_OK;
}
