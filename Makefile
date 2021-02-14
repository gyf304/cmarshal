HEADERS=$(wildcard src/*.h)
SRCS=$(wildcard src/*.c)

cmarshal: $(HEADERS) $(SRCS)
	clang -g -I/usr/lib/llvm-10/include -lclang -lstdc++ \
		$(SRCS) \
		-o cmarshal
