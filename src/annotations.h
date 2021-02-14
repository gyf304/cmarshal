#ifndef _ANNOTATIONS_H
#define _ANNOTATIONS_H

#include "./common.h"

CMarshalConfig *getCMarshalConfig(CXCursor c);
void freeCMarshalConfig(CMarshalConfig *anno);

CMarshalMemberAnnotation *getCMarshalMemberAnnotation(CXCursor c);
void freeCMarshalMemberAnnotation(CMarshalMemberAnnotation *anno);

CMarshalTypeAnnotation *getCMarshalTypeAnnotation(CXType c);
void freeCMarshalTypeAnnotation(CMarshalTypeAnnotation *anno);

#endif
