#ifndef _ALLOCATION_H
#define _ALLOCATION_H

#include <stddef.h>

typedef struct {
	int size;
	int capacity;
	void **pointers;
} AllocationContext;

AllocationContext *createAllocationContext(size_t capacity);
void destroyAllocationContext(AllocationContext *ctx);
void *allocateFromContext(AllocationContext *ctx, size_t size);

#endif
