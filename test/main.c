#include <stdlib.h>
#include <stdio.h>

#include "./main.h"

void *allocations[1024] = {0};
int allocation_count = 0;

void *unmarshal_allocator (void *context, long size) {
	void *ptr = malloc(size);
	allocations[allocation_count++] = ptr;
	return ptr;
}

int main() {
	struct test t1 = {0};
	t1.b = malloc(sizeof(t1.b) * 2);
	t1.b[0] = 1;
	t1.b[1] = 2;
	t1.bLen = 2;
	t1.c = "asdf";
	test2_t t2 = {0};
	t2.test = &t1;


	cJSON* json = marshal_test2_t(&t2);
	char *jsonStr = cJSON_Print(json);
	printf("%s\n", jsonStr);

	test2_t t2out = {0};
	unmarshal_test2_t(&t2out, json, unmarshal_allocator, NULL);
	printf("%s: %d\n", "t2out.test->a", t2out.test->a);
	printf("%s: %d\n", "t2out.test->b[0]", t2out.test->b[0]);

	for (int i = 0; i < allocation_count; i++) {
		free(allocations[i]);
	}

	cJSON_Delete(json);
	free(jsonStr);
}
