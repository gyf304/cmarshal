HEADERS=$(wildcard src/*.h)
SRCS=$(wildcard src/*.c)

demo: $(HEADERS) $(SRCS)
	clang -g -lstdc++ \
		$(SRCS) \
		-o demo
