HEADERS=$(wildcard src/*.h)
SRCS=$(wildcard src/*.c)

demo: $(HEADERS) $(SRCS)
	clang -g -Wall -lstdc++ \
		$(SRCS) \
		-o demo
