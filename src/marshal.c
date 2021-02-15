#include "./marshal.h"

#include <ctype.h>
#include <string.h>

#include "annotations.h"

/* tab indented, new-lined printf */
#define tfprintf(l, f, ...) \
	do { int ll = l; for (int ii = 0; ii < ll; ii++) { fputc('\t', f); } fprintf(f, __VA_ARGS__); fputc('\n', f); } while (0)

typedef struct {
	FILE* out;
	const char *prefix;
	const char *uppercasedPrefix;
	int unmarshal;
} MarshalData;

static char *uppercase(const char *s) {
	int len = strlen(s);
	char *uppercased = malloc(len + 1);
	for (int i = 0; i < len; i++) {
		uppercased[i] = toupper(s[i]);
	}
	uppercased[len] = 0;
	return uppercased;
}

static int isTypedefOfNamelessRecord(CXType type) {
	CXCursor typeDecl = clang_getTypeDeclaration(type);
	CXCursor childTypeDecl = getFirstChild(typeDecl);
	CXType childType = clang_getCursorType(childTypeDecl);
	if (type.kind != CXType_Typedef || childType.kind != CXType_Record) {
		return 0;
	}
	int nameless = 0;
	CXString s = clang_getCursorSpelling(childTypeDecl);
	if (strlen(clang_getCString(s)) == 0) {
		nameless = 1;
	}
	clang_disposeString(s);

	return nameless;
}

enum GenerateStatus genHeaderPreamble(FILE *out, CMarshalConfig *cfg, int unmarshal) {
	const char *prefix = unmarshal ? cfg->unmarshalerPrefix : cfg->marshalerPrefix;
	char *uppercased = uppercase(prefix);
	int l = 0;
	if (unmarshal) {
		tfprintf(l++, out, "enum %s_status {", prefix);
		tfprintf(l,   out, "%s_OK = 0,", uppercased);
		tfprintf(l,   out, "%s_ERROR_UNSPECIFIED,", uppercased);
		tfprintf(l,   out, "%s_ERROR_CONFLICTING_TYPES,", uppercased);
		tfprintf(l,   out, "%s_ERROR_CANNOT_ALLOCATE", uppercased);
		tfprintf(--l, out, "};");
		tfprintf(l,   out, "typedef void *(%s_allocator_t) (void *context, size_t size);", prefix);
	}
	free(uppercased);
	return GENERATE_OK;
};

static enum CXChildVisitResult collectFields(CXCursor c, CXCursor parent, CXClientData client_data) {
	/* unpack clientData */
	MarshalData *config = client_data;
	const char *prefix = config->prefix;
	const char *uppercasedPrefix = config->uppercasedPrefix;
	FILE* out = config->out;
	int unmarshal = config->unmarshal;

	/* getType of member */
	CXType type = clang_getCursorType(c);

	/* set up variables controlling member behaviour */
	char *nameStr = NULL;
	char *quotedKeyStr = NULL;
	char *lenField = NULL;
	char *opName = NULL;

	{ /* retrieve default values from type info */
		CXString name = clang_getCursorSpelling(c);

		nameStr = strdup(clang_getCString(name));
		quotedKeyStr = quoteString(nameStr);
		opName = getPrefixedEscapedTypeName(prefix, type);

		clang_disposeString(name);
	}

	{ /* override default values from annotation */
		CMarshalMemberAnnotation *annotation = getCMarshalMemberAnnotation(c);
		if (annotation) {
			if (annotation->length != NULL) {
				lenField = strdup(annotation->length);
			}

			if (annotation->key != NULL) {
				free(quotedKeyStr);
				quotedKeyStr = quoteString(annotation->key);
			}

			if (annotation->ignore) {
				free(quotedKeyStr);
				quotedKeyStr = NULL;
			}
			freeCMarshalMemberAnnotation(annotation);
		}
	}

	if (quotedKeyStr) {
		int l = 1;
		if (unmarshal) {
			tfprintf(l,   out, "item = cJSON_GetObjectItemCaseSensitive(src, %s);", quotedKeyStr);
			tfprintf(l++, out, "if (item != NULL) {");
			if (lenField) { // array-like
				CXType pointeeType = clang_getPointeeType(type);
				char *pointeeOpName = getPrefixedEscapedTypeName(prefix, pointeeType);

				tfprintf(l, out, "if (!cJSON_IsArray(item)) {");
				tfprintf(l, out, "	return %s_ERROR_CONFLICTING_TYPES;", uppercasedPrefix);
				tfprintf(l, out, "}");

				tfprintf(l, out, "int size = cJSON_GetArraySize(item);");
				tfprintf(l, out, "dst->%s = size;", lenField);
				tfprintf(l, out, "if (allocator == NULL)");
				tfprintf(l, out, "\treturn %s_ERROR_CANNOT_ALLOCATE;", uppercasedPrefix);
				tfprintf(l, out, "dst->%s = allocator(allocator_context, size * sizeof(*dst->%s));\n", nameStr, nameStr);
				tfprintf(l, out, "if (dst->%s == NULL)", nameStr);
				tfprintf(l, out, "\treturn %s_ERROR_CANNOT_ALLOCATE;", uppercasedPrefix);
				tfprintf(l, out, "for (int i = 0; i < size; i++) {");
				tfprintf(l, out, "\tstatus = %s(&dst->%s[i], cJSON_GetArrayItem(item, i), allocator, allocator_context);\n", pointeeOpName, nameStr);
				tfprintf(l, out, "\tif (status != %s_OK)", uppercasedPrefix);
				tfprintf(l, out, "\t\treturn status;");
				tfprintf(l, out, "}");

				free(pointeeOpName);
			} else {
				tfprintf(l, out, "status = %s(&dst->%s, item, allocator, allocator_context);", opName, nameStr);
				tfprintf(l, out, "if (status != %s_OK)", uppercasedPrefix);
				tfprintf(l, out, "	return status;");
			}
			fprintf(out, "\t}\n");
		} else {
			if (lenField) { // array-like
				CXType pointeeType = clang_getPointeeType(type);
				char *pointeeMarshalerName = getPrefixedEscapedTypeName(prefix, pointeeType);
				tfprintf(l, out, "{ /* field %s */", nameStr);
				tfprintf(l, out, "\tcJSON* arr = cJSON_CreateArray();");
				tfprintf(l, out, "\tfor (int i = 0; i < (int)(src->%s); i++) {", lenField);
				tfprintf(l, out, "\t\tcJSON_AddItemToArray(arr, %s(&src->%s[i]));", pointeeMarshalerName, nameStr);
				tfprintf(l, out, "\t}");
				tfprintf(l, out, "\tcJSON_AddItemToObject(obj, %s, arr);", quotedKeyStr);
				tfprintf(l, out, "}");
				free(pointeeMarshalerName);
			} else {
				tfprintf(l, out, "cJSON_AddItemToObject(obj, %s, %s(&src->%s));", quotedKeyStr, opName, nameStr);
			}
		}
	}

	free(nameStr);
	free(quotedKeyStr);
	free(lenField);
	free(opName);

	return CXChildVisit_Continue;
}

/*
 * C code generators
 */
enum GenerateStatus genForwardDecl(FILE *out, CMarshalConfig *cfg, CXType type, const char *specifiers, int unmarshal)
{
	if (out == NULL) {
		return GENERATE_OK;
	}

	if (isTypedefOfNamelessRecord(type)) {
		return GENERATE_OK;
	}

	const char *prefix = unmarshal ? cfg->unmarshalerPrefix : cfg->marshalerPrefix;

	char *uppercasedPrefix = uppercase(prefix);
	CXString typeSpelling = clang_getTypeSpelling(type);
	const char *typeSpellingStr = clang_getCString(typeSpelling);
	char *opName = getPrefixedEscapedTypeName(prefix, type);

	if (unmarshal) {
		fprintf(
			out,
			"%s%senum %s_status %s(%s* dst, const cJSON* src, %s_allocator_t* allocator, void* allocator_context);\n",
			specifiers ? specifiers : "",
			specifiers ? " " : "",
			prefix,
			opName,
			typeSpellingStr,
			prefix
		);
	} else {
		fprintf(
			out,
			"%s%scJSON* %s(%s* src);\n",
			specifiers ? specifiers : "",
			specifiers ? " " : "",
			opName,
			typeSpellingStr
		);
	}

	free(opName);
	clang_disposeString(typeSpelling);
	free(uppercasedPrefix);

	return GENERATE_OK;
}

enum GenerateStatus genImplPreamble(FILE *out, CMarshalConfig *cfg, int unmarshal) {
	if (out == NULL) {
		return GENERATE_OK;
	}
	if (unmarshal) {
		fprintf(out, "#include <string.h>\n");
	}
	return GENERATE_OK;
}

enum GenerateStatus genImpl(FILE *out, CMarshalConfig *cfg, CXType type, const char *specifiers, int unmarshal)
{
	if (out == NULL) {
		return GENERATE_OK;
	}

	if (isTypedefOfNamelessRecord(type)) {
		return GENERATE_OK;
	}

	enum GenerateStatus result = GENERATE_OK;
	const char *prefix = unmarshal ? cfg->unmarshalerPrefix : cfg->marshalerPrefix;

	char *uppercasedPrefix = uppercase(prefix);
	CXString typeSpelling = clang_getTypeSpelling(type);
	const char *typeSpellingStr = clang_getCString(typeSpelling);
	char *opName = getPrefixedEscapedTypeName(prefix, type);

	if (unmarshal) {
		fprintf(
			out,
			"%s%senum %s_status %s(%s* dst, const cJSON* src, %s_allocator_t* allocator, void* allocator_context) \n{\n",
			specifiers ? specifiers : "",
			specifiers ? " " : "",
			prefix,
			opName,
			typeSpellingStr,
			prefix
		);
	} else {
		fprintf(
			out,
			"%s%scJSON* %s(%s* src) \n{\n",
			specifiers ? specifiers : "",
			specifiers ? " " : "",
			opName,
			typeSpellingStr
		);
	}

	int l = 1;

	switch (type.kind)
	{
	case CXType_Bool:
		if (unmarshal) {
			tfprintf(l, out, "if (!cJSON_IsBool(src))");
			tfprintf(l, out, "\treturn %s_ERROR_CONFLICTING_TYPES;", uppercasedPrefix);
			tfprintf(l, out, "*dst = cJSON_IsTrue(src);");
			tfprintf(l, out, "return %s_OK;", uppercasedPrefix);
		} else {
			tfprintf(l, out, "return cJSON_CreateBool(*src);");
		}
		break;
	case CXType_UShort:
	case CXType_UInt:
	case CXType_ULong:
	case CXType_ULongLong:
	case CXType_UInt128:
	case CXType_Short:
	case CXType_Int:
	case CXType_Long:
	case CXType_LongLong:
	case CXType_Int128:
	case CXType_Float:
	case CXType_Double:
	case CXType_Half:
	case CXType_Float16:
		if (unmarshal) {
			tfprintf(l, out, "if (!cJSON_IsNumber(src))");
			tfprintf(l, out, "\treturn %s_ERROR_CONFLICTING_TYPES;", uppercasedPrefix);
			tfprintf(l, out, "*dst = (%s)cJSON_GetNumberValue(src);", typeSpellingStr);
			tfprintf(l, out, "return %s_OK;", uppercasedPrefix);
		} else {
			tfprintf(l, out, "return cJSON_CreateNumber((double) *src);");
		}
		break;
	case CXType_Pointer: {
		CXType pointeeType = clang_getPointeeType(type);

		if (unmarshal) {
			tfprintf(l, out, "if (cJSON_IsNull(src)) {");
			tfprintf(l, out, "\t*dst = NULL;");
			tfprintf(l, out, "\treturn %s_OK;", uppercasedPrefix);
			tfprintf(l, out, "}");

			if (pointeeType.kind == CXType_Char_S || pointeeType.kind == CXType_Char_U) {
				tfprintf(l, out, "if (!cJSON_IsString(src))");
				tfprintf(l, out, "\treturn %s_ERROR_CONFLICTING_TYPES;", uppercasedPrefix);
				tfprintf(l, out, "char *s = cJSON_GetStringValue(src);");
				tfprintf(l, out, "size_t len = strlen(s);");
				tfprintf(l, out, "if (allocator == NULL)");
				tfprintf(l, out, "\treturn %s_ERROR_CANNOT_ALLOCATE;", uppercasedPrefix);
				tfprintf(l, out, "*dst = allocator(allocator_context, len+1);");
				tfprintf(l, out, "if (*dst == NULL)");
				tfprintf(l, out, "\treturn %s_ERROR_CANNOT_ALLOCATE;", uppercasedPrefix);
				tfprintf(l, out, "memcpy(*dst, s, len+1);");
				tfprintf(l, out, "return %s_OK;", uppercasedPrefix);
			} else {
				char *opName = getPrefixedEscapedTypeName(prefix, pointeeType);
				tfprintf(l, out, "if (allocator == NULL)");
				tfprintf(l, out, "\treturn %s_ERROR_CANNOT_ALLOCATE;", uppercasedPrefix);
				tfprintf(l, out, "*dst = allocator(allocator_context, sizeof(**dst));");
				tfprintf(l, out, "if (*dst == NULL)");
				tfprintf(l, out, "\treturn %s_ERROR_CANNOT_ALLOCATE;", uppercasedPrefix);
				tfprintf(l, out, "return %s(*dst, src, allocator, allocator_context);", opName);
				free(opName);
			}
		} else {
			tfprintf(l, out, "if (*src == NULL)");
			tfprintf(l, out, "\treturn cJSON_CreateNull();");
			if (pointeeType.kind == CXType_Char_S || pointeeType.kind == CXType_Char_U) {
				tfprintf(l, out, "return cJSON_CreateString(*src);");
			} else {
				char *opName = getPrefixedEscapedTypeName(prefix, pointeeType);
				tfprintf(l, out, "return %s(*src);", opName);
				free(opName);
			}
		}
		break;
	}
	case CXType_Typedef: {
		CXCursor child = getFirstChild(clang_getTypeDeclaration(type));
		CXType actualType = clang_getCursorType(child);
		if (actualType.kind == CXType_Invalid) {
			actualType = clang_getTypedefDeclUnderlyingType(clang_getTypeDeclaration(type));
		}
		char *opName = getPrefixedEscapedTypeName(prefix, actualType);
		if (unmarshal) {
			tfprintf(l, out, "return %s(dst, src, allocator, allocator_context);", opName);
		} else {
			tfprintf(l, out, "return %s(src);", opName);
		}
		free(opName);
		break;
	}
	case CXType_Record: {
		if (clang_Cursor_isAnonymous(clang_getTypeDeclaration(type))) {
			fprintf(stderr, "Failed to generate for type %s: unsupported anonymous type.\n", typeSpellingStr);
			result = GENERATE_ERROR_UNSUPPORTED_TYPE;
			break;
		}
		// a little bit complicated this one.
		CXCursor c = clang_getTypeDeclaration(type);
		MarshalData data = {out, prefix, uppercasedPrefix, unmarshal};
		if (unmarshal) {
			tfprintf(l, out, "if (!cJSON_IsObject(src))");
			tfprintf(l, out, "\treturn %s_ERROR_CONFLICTING_TYPES;", uppercasedPrefix);
			tfprintf(l, out, "cJSON *item = NULL;");
			tfprintf(l, out, "enum %s_status status = %s_OK;", prefix, uppercasedPrefix);
			clang_visitChildren(c, collectFields, (void *)&data);
			tfprintf(l, out, "return %s_OK;", uppercasedPrefix);
		} else {
			tfprintf(l, out, "cJSON* obj = cJSON_CreateObject();");
			clang_visitChildren(c, collectFields, (void *)&data);
			tfprintf(l, out, "return obj;");
		}
		break;
	}
	default:
		fprintf(stderr, "Failed to generate for type %s: unsupported.\n", typeSpellingStr);
		result = GENERATE_ERROR_UNSUPPORTED_TYPE;
		break;
	}
	tfprintf(0, out, "}");

	free(opName);
	clang_disposeString(typeSpelling);
	free(uppercasedPrefix);

	return result;
}
