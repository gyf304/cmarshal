HEADERS=$(wildcard src/*.h)
SRCS=$(wildcard src/*.c)

.PHONY: regenerate

demo: $(HEADERS) $(SRCS)
	$(CC) -g -Wall -Wno-unused-function -lstdc++ \
		$(SRCS) \
		-o demo

regenerate:
	cd src && ../../cmarshal -DCMARSHAL_GENERATE ./demo.h
