#ifndef _UNMARSHAL_H
#define _UNMARSHAL_H

#include "./common.h"

void genUnmarshalUtils(FILE *out, const char *prefix);
void genUnmarshalerForwardDecl(CXType type, FILE *out, const char *prefix, const char *specifiers);
void genUnmarshalerImplPreamble(FILE *out, const char *prefix);
void genUnmarshalerImpl(CXType type, FILE *out, const char *prefix, const char *specifiers);

#endif
