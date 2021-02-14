#ifndef _TYPEFIND_H
#define _TYPEFIND_H

#include "./common.h"

typedef void (TypeCallback) (void *context, CXType type, CMarshalTypeAnnotation *anno);
void findAnnotatedTypes(CXCursor c, TypeCallback *cb, void *context);
void findDependentTypes(CXType type, TypeCallback *cb, void *context);

typedef void (ConfigCallback) (void *context, CMarshalConfig *config);
int findConfig(CXCursor c, ConfigCallback *cb, void *context);

#endif
