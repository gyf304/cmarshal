/* Auto-generated, do not edit */
#include "./cJSON.h"
#include "./specs/test.h"

/*
 * Exported Marshaler Function Declarations 
 */
/* marshaler for type test_t */
cJSON* marshal_test_t(test_t* src);

/* marshaler for type test2_t */
cJSON* marshal_test2_t(test2_t* src);

/* Auto-generated, do not edit */
/*
 * Unexported Marshaler Function Declarations 
 */
/* marshaler for type struct test */
static cJSON* marshal_struct_test(struct test* src);

/* marshaler for type struct test * */
static cJSON* marshal_struct_test_p(struct test ** src);

/* marshaler for type _Bool */
static cJSON* marshal__Bool(_Bool* src);

/* marshaler for type int * */
static cJSON* marshal_int_p(int ** src);

/* marshaler for type int */
static cJSON* marshal_int(int* src);

/* marshaler for type char * */
static cJSON* marshal_char_p(char ** src);

/*
 * Marshaler Function Implementations 
 */ 
cJSON* marshal_test_t(test_t* src) 
{
	return marshal_struct_test((void *) src);
}

cJSON* marshal_test2_t(test2_t* src) 
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddItemToObject(obj, "test", marshal_struct_test_p(&src->test));
	return obj;
}

static cJSON* marshal_struct_test(struct test* src) 
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddItemToObject(obj, "a", marshal__Bool(&src->a));
	{ /* field b */
		cJSON* arr = cJSON_CreateArray();
		for (int i = 0; i < (int)(src->bLen); i++) {
			cJSON_AddItemToArray(arr, marshal_int(&src->b[i]));
		}
		cJSON_AddItemToObject(obj, "b", arr);
	}
	cJSON_AddItemToObject(obj, "asdf", marshal_char_p(&src->c));
	return obj;
}

static cJSON* marshal_struct_test_p(struct test ** src) 
{
	if (*src == NULL) {
		return cJSON_CreateNull();
	}
	return marshal_struct_test(*src);
}

static cJSON* marshal__Bool(_Bool* src) 
{
	return cJSON_CreateBool(*src);
}

static cJSON* marshal_int_p(int ** src) 
{
	if (*src == NULL) {
		return cJSON_CreateNull();
	}
	return marshal_int(*src);
}

static cJSON* marshal_int(int* src) 
{
	return cJSON_CreateNumber((double) *src);
}

static cJSON* marshal_char_p(char ** src) 
{
	if (*src == NULL) {
		return cJSON_CreateNull();
	}
	return cJSON_CreateString(*src);
}

/* Auto-generated, do not edit */
#include "./cJSON.h"
#include "./specs/test.h"

enum unmarshal_status {
	UNMARSHAL_OK = 0,
	UNMARSHAL_ERROR_UNSPECIFIED,
	UNMARSHAL_ERROR_CONFLICTING_TYPES,
	UNMARSHAL_ERROR_CANNOT_ALLOCATE,
	UNMARSHAL_ERROR_MISSING_REQUIRED_KEY
};
typedef void *(unmarshal_allocator_t) (void *context, long size);
/*
 * Exported Unmarshaler Function Declarations 
 */
/* unmarshaler for type test_t */
enum unmarshal_status unmarshal_test_t(test_t* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/* unmarshaler for type test2_t */
enum unmarshal_status unmarshal_test2_t(test2_t* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/* Auto-generated, do not edit */
#include <string.h>
/*
 * Unexported Unmarshaler Function Declarations 
 */
/* unmarshaler for type struct test */
static enum unmarshal_status unmarshal_struct_test(struct test* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/* unmarshaler for type struct test * */
static enum unmarshal_status unmarshal_struct_test_p(struct test ** dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/* unmarshaler for type _Bool */
static enum unmarshal_status unmarshal__Bool(_Bool* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/* unmarshaler for type int * */
static enum unmarshal_status unmarshal_int_p(int ** dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/* unmarshaler for type int */
static enum unmarshal_status unmarshal_int(int* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/* unmarshaler for type char * */
static enum unmarshal_status unmarshal_char_p(char ** dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/*
 * Unmarshaler Function Implementations 
 */ 
enum unmarshal_status unmarshal_test_t(test_t* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	return unmarshal_struct_test(dst, src, allocator, allocator_context);
	return UNMARSHAL_OK;
}

enum unmarshal_status unmarshal_test2_t(test2_t* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (!cJSON_IsObject(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	cJSON *item = NULL;
	enum unmarshal_status status = UNMARSHAL_OK;
	item = cJSON_GetObjectItemCaseSensitive(src, "test");
	status = unmarshal_struct_test_p(&dst->test, item, allocator, allocator_context);
	if (status != UNMARSHAL_OK)
		return status;
	return UNMARSHAL_OK;
}

static enum unmarshal_status unmarshal_struct_test(struct test* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (!cJSON_IsObject(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	cJSON *item = NULL;
	enum unmarshal_status status = UNMARSHAL_OK;
	item = cJSON_GetObjectItemCaseSensitive(src, "a");
	status = unmarshal__Bool(&dst->a, item, allocator, allocator_context);
	if (status != UNMARSHAL_OK)
		return status;
	item = cJSON_GetObjectItemCaseSensitive(src, "b");
	if (!cJSON_IsArray(item)) {
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	}
	int size = cJSON_GetArraySize(item);
	dst->bLen = size;
	if (allocator == NULL)
		return UNMARSHAL_ERROR_CANNOT_ALLOCATE;
	dst->b = allocator(allocator_context, size * sizeof(*dst->b));
	if (dst->b == NULL)
		return UNMARSHAL_ERROR_CANNOT_ALLOCATE;
	for (int i = 0; i < size; i++) {
		status = unmarshal_int(&dst->b[i], cJSON_GetArrayItem(item, i), allocator, allocator_context);
		if (status != UNMARSHAL_OK)
			return status;
	}
	item = cJSON_GetObjectItemCaseSensitive(src, "asdf");
	status = unmarshal_char_p(&dst->c, item, allocator, allocator_context);
	if (status != UNMARSHAL_OK)
		return status;
	return UNMARSHAL_OK;
}

static enum unmarshal_status unmarshal_struct_test_p(struct test ** dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (cJSON_IsNull(src)) {
		*dst = NULL;
		return UNMARSHAL_OK;
	}
	if (allocator == NULL)
		return UNMARSHAL_ERROR_CANNOT_ALLOCATE;
	*dst = allocator(allocator_context, sizeof(**dst));
	if (*dst == NULL)
		return UNMARSHAL_ERROR_CANNOT_ALLOCATE;
	return unmarshal_struct_test(*dst, src, allocator, allocator_context);
	return UNMARSHAL_OK;
}

static enum unmarshal_status unmarshal__Bool(_Bool* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (!cJSON_IsBool(src))
		return UNMARSHAL_ERROR_CONFLICTING_TYPES;
	*dst = cJSON_IsTrue(src);
	return UNMARSHAL_OK;
}

static enum unmarshal_status unmarshal_int_p(int ** dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context) 
{
	if (cJSON_IsNull(src)) {
		*dst = NULL;
		return UNMARSHAL_OK;
	}
	if (allocator == NULL)
		return UNMARSHAL_ERROR_CANNOT_ALLOCATE;
	*dst = allocator(allocator_context, sizeof(**dst));
	if (*dst == NULL)
		return UNMARSHAL_ERROR_CANNOT_ALLOCATE;
	return unmarshal_int(*dst, src, allocator, allocator_context);
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
	memcpy(dst, s, len+1);
	return UNMARSHAL_OK;
}

