/* auto-generated, do not edit */
#ifndef _CMARSHAL_UNMARSHALER_1804289383
#define _CMARSHAL_UNMARSHALER_1804289383
#include "./cJSON.h"
#include "./demo.h"
enum unmarshal_status {
	UNMARSHAL_OK = 0,
	UNMARSHAL_ERROR_UNSPECIFIED,
	UNMARSHAL_ERROR_CONFLICTING_TYPES,
	UNMARSHAL_ERROR_CANNOT_ALLOCATE
};
typedef void *(unmarshal_allocator_t) (void *context, size_t size);
enum unmarshal_status unmarshal_MyStruct(MyStruct* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
#endif
