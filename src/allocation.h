#ifndef _ALLOCATION_H
#define _ALLOCATION_H

#include <stddef.h>

typedef void (ForEachAllocationCallback) (void *ctx, void *ptr);

typedef struct {
	int size;
	int capacity;
	void **pointers;
} AllocationContext;

AllocationContext *createAllocationContext(size_t capacity);
void destroyAllocationContext(AllocationContext *ctx);
void *allocateFromContext(AllocationContext *ctx, size_t size);
void forEachAllocation(AllocationContext *ctx, ForEachAllocationCallback *cb, void *cbctx);

#endif
