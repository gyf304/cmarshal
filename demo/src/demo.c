#include "demo.h"
#include "demo.marshaler.h"
#include "demo.unmarshaler.h"

#include <stdio.h>
#include <stdlib.h>

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
	cJSON *json = marshal_MyStruct(&s);
	char *str = cJSON_Print(json);
	printf("%s\n", str);
	free(str);
	cJSON_Delete(json);
}

static void demo_unmarshal() {
	allocation_count = 0;
	MyStruct s = {0};
	cJSON *json = cJSON_Parse("{\"hello\": 321, \"world\": \"test\"}");
	unmarshal_MyStruct(&s, json, unmarshal_allocator, NULL);
	cJSON_Delete(json);
	printf("allocated: %d block(s) of memory\n", allocation_count);
	printf("hello: %d\nworld: %s\n", s.hello, s.world);
	for (int i = 0; i < allocation_count; i++) {
		free(allocations[i]);
	}
	allocation_count = 0;
}

int main(int argc, const char *argv[]) {
	demo_marshal();
	demo_unmarshal();
}
