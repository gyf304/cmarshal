/* Auto-generated, do not edit */
#include "cJSON.h"
#include "src/jsonstructs.h"

enum unmarshal_status {
	UNMARSHAL_OK = 0,
	UNMARSHAL_ERROR_UNSPECIFIED,
	UNMARSHAL_ERROR_CONFLICTING_TYPES,
	UNMARSHAL_ERROR_CANNOT_ALLOCATE,
	UNMARSHAL_ERROR_MISSING_REQUIRED_KEY
};
typedef void *(unmarshal_allocator_t) (void *context, size_t size);
/*
 * Exported Unmarshaler Function Declarations 
 */
/* unmarshaler for type CMarshalConfig */
enum unmarshal_status unmarshal_CMarshalConfig(CMarshalConfig* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/* unmarshaler for type CMarshalTypeAnnotation */
enum unmarshal_status unmarshal_CMarshalTypeAnnotation(CMarshalTypeAnnotation* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
/* unmarshaler for type CMarshalMemberAnnotation */
enum unmarshal_status unmarshal_CMarshalMemberAnnotation(CMarshalMemberAnnotation* dst, const cJSON* src, unmarshal_allocator_t* allocator, void* allocator_context);
