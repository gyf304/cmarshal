#ifndef _COMMON_H
#define _COMMON_H

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <clang-c/Index.h>
#include <string.h>
#include <stdlib.h>

#include "./cJSON.h"
#include "./jsonstructs.h"

char *getPrefixedEscapedTypeName(const char *prefix, CXType type);
CXCursor getFirstChild(CXCursor c);
char *quoteString(const char *s);

#endif
