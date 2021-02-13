HEADERS=$(wildcard src/*.h)
SRCS=$(wildcard src/*.c)

cmarshal: $(HEADERS) $(SRCS)
	clang -g -I/usr/lib/llvm-10/include -lclang -lstdc++ \
		$(SRCS) \
		-o cmarshal

test/main.h: cmarshal
	cd test && ../cmarshal ./specs/test.h > main.h

demo: test/main.h test/main.c
	clang test/cJSON.c test/main.c -o demo
