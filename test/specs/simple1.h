#ifndef _SIMPLE_1
#define _SIMPLE_1

#include <stdbool.h>

struct test {
    bool a;
    int *b;   /* `{"cmarshal": {"length": "bLen"}}` */
    int bLen; /* `{"cmarshal": {"ignore": true}}` */
    char *c;  /* `{"cmarshal": {"key": "asdf"}}` */
};

/* `{"cmarshal": true}` */
typedef struct test test_t;

#endif
