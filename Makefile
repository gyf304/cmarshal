HEADERS=$(wildcard src/*.h)
SRCS=$(wildcard src/*.c)

# don't be me, don't compile like this
cmarshal: $(HEADERS) $(SRCS)
	clang -std=c99 -Os -Wall -I/usr/lib/llvm-10/include -lclang -lstdc++ \
		$(SRCS) \
		-o cmarshal

regenerate: cmarshal
	cd src && ../cmarshal -DCMARSHAL_GENERATE ./jsonstructs.h
