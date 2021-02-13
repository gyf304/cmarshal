#ifndef _MARSHAL_H
#define _MARSHAL_H

#include "./common.h"

void genMarshalerForwardDecl(CXType type, FILE *out, const char *prefix, const char *specifiers);
void genMarshalerImpl(CXType type, FILE *out, const char *prefix, const char *specifiers);

#endif
