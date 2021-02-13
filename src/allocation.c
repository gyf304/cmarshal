#include "./allocation.h"
#include <stdlib.h>

AllocationContext *createAllocationContext(size_t capacity) {
	AllocationContext *ctx = malloc(sizeof(AllocationContext));
	ctx->capacity = capacity;
	ctx->pointers = malloc(capacity);
	ctx->size = 0;
	return ctx;
}

void destroyAllocationContext(AllocationContext *ctx) {
	for (size_t i = 0; i < ctx->size; i++) {
		free(ctx->pointers[i]);
	}
	free(ctx->pointers);
	free(ctx);
}

void *allocateFromContext(AllocationContext *ctx, size_t size) {
	if (ctx->size >= ctx->capacity) {
		return NULL;
	}
	void *ptr = malloc(size);
	ctx->pointers[ctx->size++] = ptr;
	return ptr;
}
