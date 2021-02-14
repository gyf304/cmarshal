#include "./allocation.h"
#include <stdlib.h>

#define MIN_CAPACITY 16

AllocationContext *createAllocationContext(size_t capacity) {
	AllocationContext *ctx = malloc(sizeof(AllocationContext));
	ctx->capacity = capacity > MIN_CAPACITY ? capacity : MIN_CAPACITY;
	ctx->pointers = malloc(ctx->capacity * sizeof(void *));
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
		ctx->capacity = ctx->capacity * 2;
		ctx->pointers = realloc(ctx->pointers, ctx->capacity);
	}
	void *ptr = malloc(size);
	ctx->pointers[ctx->size++] = ptr;
	return ptr;
}

void forEachAllocation(AllocationContext *ctx, ForEachAllocationCallback *cb, void *cbctx) {
	for (size_t i = 0; i < ctx->size; i++) {
		cb(cbctx, ctx->pointers[i]);
	}
}
