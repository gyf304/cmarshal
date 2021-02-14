#include "demo.h"
#include "demo.marshaler.h"
#include "demo.unmarshaler.h"

#include <stdio.h>
#include <stdlib.h>

int arr[] = {1, 2, 3};

void *allocations[64] = {0};
int allocation_count = 0;

void *unmarshal_allocator(void *context, size_t size) {
	void *ptr = malloc(size);
	allocations[allocation_count++] = ptr;
	return ptr;
}

static void demo_marshal() {
	MyStruct s = {0};

	s.hello = 123;
	s.world = "yo";

	s.arr = arr;
	s.len = 3;

	MyStruct2 s2 = {0};
	s2.a = 1;
	s2.b = 2;

	s.nested = &s2;

	cJSON *json = marshal_MyStruct(&s);
	char *str = cJSON_Print(json);
	printf("%s\n", str);
	free(str);
	cJSON_Delete(json);
}

static void demo_unmarshal() {
	allocation_count = 0;
	MyStruct s = {0};

	cJSON *json = cJSON_Parse("{\"hello\": 321, \"world\": \"test\", \"nested\": {\"a\": 2, \"b\": 1}}");
	unmarshal_MyStruct(&s, json, unmarshal_allocator, NULL);
	cJSON_Delete(json);

	printf("allocated: %d block(s) of memory\n", allocation_count);
	printf("hello: %d\nworld: %s\nnested.a: %d\nnested.b: %d\n", s.hello, s.world, s.nested->a, s.nested->b);

	for (int i = 0; i < allocation_count; i++) {
		free(allocations[i]);
	}

	allocation_count = 0;
}

int main(int argc, const char *argv[]) {
	demo_marshal();
	demo_unmarshal();
}
