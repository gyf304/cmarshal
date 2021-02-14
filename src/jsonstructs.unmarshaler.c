#include "./jsonstructs.unmarshaler.h"
#include <string.h>
static enum unmarshal_status unmarshal_char_p(char ** dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
static enum unmarshal_status unmarshal__Bool(_Bool* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
enum unmarshal_status unmarshal_CMarshalConfig(CMarshalConfig* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (!cJSON_IsObject(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	cJSON *item = NULL;
	enum unmarshal_status status = UNMARSHAL_OK;
	item = cJSON_GetObjectItemCaseSensitive(src, "cJSONInclude");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->cJSONInclude, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "marshalerPrefix");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->marshalerPrefix, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "unmarshalerPrefix");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->unmarshalerPrefix, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "marshalerHeaderFile");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->marshalerHeaderFile, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "marshalerImplFile");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->marshalerImplFile, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "unmarshalerHeaderFile");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->unmarshalerHeaderFile, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "unmarshalerImplFile");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->unmarshalerImplFile, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	return UNMARSHAL_OK;
}
enum unmarshal_status unmarshal_CMarshalTypeAnnotation(CMarshalTypeAnnotation* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (!cJSON_IsObject(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	cJSON *item = NULL;
	enum unmarshal_status status = UNMARSHAL_OK;
	item = cJSON_GetObjectItemCaseSensitive(src, "marshal");
	if (item != NULL) {
		status = unmarshal__Bool(&dst->marshal, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "unmarshal");
	if (item != NULL) {
		status = unmarshal__Bool(&dst->unmarshal, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	return UNMARSHAL_OK;
}
enum unmarshal_status unmarshal_CMarshalMemberAnnotation(CMarshalMemberAnnotation* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (!cJSON_IsObject(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	cJSON *item = NULL;
	enum unmarshal_status status = UNMARSHAL_OK;
	item = cJSON_GetObjectItemCaseSensitive(src, "ignore");
	if (item != NULL) {
		status = unmarshal__Bool(&dst->ignore, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "key");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->key, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "length");
	if (item != NULL) {
		status = unmarshal_char_p(&dst->length, item, allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
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
static enum unmarshal_status unmarshal__Bool(_Bool* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (!cJSON_IsBool(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	*dst = cJSON_IsTrue(src);
	return UNMARSHAL_OK;
}
