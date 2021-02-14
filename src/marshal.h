#ifndef _MARSHAL_H
#define _MARSHAL_H

#include "./common.h"

enum GenerateStatus {
	GENERATE_OK = 0,
	GENERATE_ERROR_UNSPECIFIED,
	GENERATE_ERROR_UNSUPPORTED_TYPE
};

enum GenerateStatus genHeaderPreamble(FILE *out, CMarshalConfig *cfg, int unmarshal);
enum GenerateStatus genForwardDecl(FILE *out, CMarshalConfig *cfg, CXType type, const char *specifiers, int unmarshal);
enum GenerateStatus genImplPreamble(FILE *out, CMarshalConfig *cfg, int unmarshal);
enum GenerateStatus genImpl(FILE *out, CMarshalConfig *cfg, CXType type, const char *specifiers, int unmarshal);

#endif
