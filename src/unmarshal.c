#include "./marshal.h"

#include <ctype.h>
#include <string.h>

/* tab indented, new-lined printf */
#define tfprintf(l, f, ...) do { int ll = l; for (int ii = 0; ii < ll; ii++) { fputc('\t', f); } fprintf(f, __VA_ARGS__); fputc('\n', f); } while (0)

typedef struct {
	FILE* out;
	const char *prefix;
	const char *uppercasedPrefix;
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

void genUnmarshalUtils(FILE *out, const char *prefix) {
	char *uppercased = uppercase(prefix);
	int l = 0;

	tfprintf(l++, out, "enum %s_status {", prefix);
	tfprintf(l,   out, "%s_OK = 0,", uppercased);
	tfprintf(l,   out, "%s_ERROR_UNSPECIFIED,", uppercased);
	tfprintf(l,   out, "%s_ERROR_CONFLICTING_TYPES,", uppercased);
	tfprintf(l,   out, "%s_ERROR_CANNOT_ALLOCATE,", uppercased);
	tfprintf(l,   out, "%s_ERROR_MISSING_REQUIRED_KEY", uppercased);
	tfprintf(--l, out, "};");
	tfprintf(l,   out, "typedef void *(%s_allocator_t) (void *context, size_t size);", prefix);
	free(uppercased);
};

static enum CXChildVisitResult collectFields(CXCursor c, CXCursor parent, CXClientData client_data) {
	/* unpack clientData */
	MarshalData *config = client_data;
	const char *prefix = config->prefix;
	const char *uppercasedPrefix = config->uppercasedPrefix;
	FILE* out = config->out;

	/* getType of member */
	CXType type = clang_getCursorType(c);

	/* set up variables controlling member behaviour */
	char *nameStr = NULL;
	char *quotedKeyStr = NULL;
	char *lenField = NULL;
	char *unmarshalerName = NULL;

	{ /* retrieve default values from type info */
		CXString name = clang_getCursorSpelling(c);

		nameStr = strdup(clang_getCString(name));
		quotedKeyStr = quoteString(nameStr);
		unmarshalerName = getPrefixedEscapedTypeName(prefix, type);

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
		}
		freeCMarshalMemberAnnotation(annotation);
	}

	if (quotedKeyStr) {
		int l = 1;

		tfprintf(l,   out, "item = cJSON_GetObjectItemCaseSensitive(src, %s);", quotedKeyStr);
		tfprintf(l++, out, "if (item != NULL) {");
		if (lenField) { // array-like
			CXType pointeeType = clang_getPointeeType(type);
			char *pointeeUnmarshalerName = getPrefixedEscapedTypeName(prefix, pointeeType);

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
			tfprintf(l, out, "\tstatus = %s(&dst->%s[i], cJSON_GetArrayItem(item, i), allocator, allocator_context);\n", pointeeUnmarshalerName, nameStr);
			tfprintf(l, out, "\tif (status != %s_OK)", uppercasedPrefix);
			tfprintf(l, out, "\t\treturn status;");
			tfprintf(l, out, "}");

			free(pointeeUnmarshalerName);
		} else {
			tfprintf(l, out, "status = %s(&dst->%s, item, allocator, allocator_context);", unmarshalerName, nameStr);
			tfprintf(l, out, "if (status != %s_OK)", uppercasedPrefix);
			tfprintf(l, out, "	return status;");
		}
		fprintf(out, "\t}\n");
	}

	free(nameStr);
	free(quotedKeyStr);
	free(lenField);
	free(unmarshalerName);

	return CXChildVisit_Continue;
}

/*
 * C code generators
 */
void genUnmarshalerForwardDecl(CXType type, FILE *out, const char *prefix, const char *specifiers)
{
	char *uppercasedPrefix = uppercase(prefix);
	CXString typeSpelling = clang_getTypeSpelling(type);
	CXCursor declCursor = clang_getTypeDeclaration(type);
	const char *typeSpellingStr = clang_getCString(typeSpelling);
	char *unmarshalerName = getPrefixedEscapedTypeName(prefix, type);

	fprintf(
		out,
		"/* unmarshaler for type %s */\n",
		typeSpellingStr
	);
	fprintf(
		out,
		"%s%senum %s_status %s(%s* dst, const cJSON* src, %s_allocator_t* allocator, void* allocator_context);\n",
		specifiers ? specifiers : "",
		specifiers ? " " : "",
		prefix,
		unmarshalerName,
		typeSpellingStr,
		prefix
	);

	free(unmarshalerName);
	clang_disposeString(typeSpelling);
	free(uppercasedPrefix);
}

void genUnmarshalerImplPreamble(FILE *out, const char *prefix) {
	fprintf(out, "#include <string.h>\n");
}

void genUnmarshalerImpl(CXType type, FILE *out, const char *prefix, const char *specifiers)
{
	char *uppercasedPrefix = uppercase(prefix);
	CXString typeSpelling = clang_getTypeSpelling(type);
	CXCursor declCursor = clang_getTypeDeclaration(type);
	const char *typeSpellingStr = clang_getCString(typeSpelling);
	char *unmarshalerName = getPrefixedEscapedTypeName(prefix, type);
	int suppressReturn = 0;

	fprintf(
		out,
		"%s%senum %s_status %s(%s* dst, const cJSON* src, %s_allocator_t* allocator, void* allocator_context) \n{\n",
		specifiers ? specifiers : "",
		specifiers ? " " : "",
		prefix,
		unmarshalerName,
		typeSpellingStr,
		prefix
	);

	switch (type.kind)
	{
	case CXType_Bool:
		fprintf(out, "\tif (!cJSON_IsBool(src))\n");
		fprintf(out, "\t\treturn %s_ERROR_CONFLICTING_TYPES;\n", uppercasedPrefix);
		fprintf(out, "\t*dst = cJSON_IsTrue(src);\n");
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
		fprintf(out, "\tif (!cJSON_IsNumber(src))\n");
		fprintf(out, "\t\treturn %s_ERROR_CONFLICTING_TYPES;\n", uppercasedPrefix);
		fprintf(out, "\t*dst = (%s)cJSON_GetNumberValue(src);\n", typeSpellingStr);
		break;
	case CXType_Pointer: {
		CXType pointeeType = clang_getPointeeType(type);
		fprintf(out, "\tif (cJSON_IsNull(src)) {\n\t\t*dst = NULL;\n\t\treturn %s_OK;\n\t}\n", uppercasedPrefix);
		if (pointeeType.kind == CXType_Char_S || pointeeType.kind == CXType_Char_U) {
			fprintf(out, "\tif (!cJSON_IsString(src))\n");
			fprintf(out, "\treturn %s_ERROR_CONFLICTING_TYPES;\n", uppercasedPrefix);
			fprintf(out, "\tchar *s = cJSON_GetStringValue(src);\n");
			fprintf(out, "\tsize_t len = strlen(s);\n");
			fprintf(out, "\tif (allocator == NULL)\n\t\treturn %s_ERROR_CANNOT_ALLOCATE;\n", uppercasedPrefix);
			fprintf(out, "\t*dst = allocator(allocator_context, len+1);\n");
			fprintf(out, "\tif (*dst == NULL)\n\t\treturn %s_ERROR_CANNOT_ALLOCATE;\n", uppercasedPrefix);
			fprintf(out, "\tmemcpy(*dst, s, len+1);\n");
		} else {
			char *unmarshalerName = getPrefixedEscapedTypeName(prefix, pointeeType);
			fprintf(out, "\tif (allocator == NULL)\n\t\treturn %s_ERROR_CANNOT_ALLOCATE;\n", uppercasedPrefix);
			fprintf(out, "\t*dst = allocator(allocator_context, sizeof(**dst));\n");
			fprintf(out, "\tif (*dst == NULL)\n\t\treturn %s_ERROR_CANNOT_ALLOCATE;\n", uppercasedPrefix);
			fprintf(out, "\treturn %s(*dst, src, allocator, allocator_context);\n", unmarshalerName);
			free(unmarshalerName);
			suppressReturn = 1;
		}
		break;
	}
	case CXType_Typedef: {
		CXCursor child = getFirstChild(clang_getTypeDeclaration(type));
		CXType actualType = clang_getCursorType(child);
		char *unmarshalerName = getPrefixedEscapedTypeName(prefix, actualType);
		fprintf(out, "\treturn %s(dst, src, allocator, allocator_context);\n", unmarshalerName);
		free(unmarshalerName);
		suppressReturn = 1;
		break;
	}
	case CXType_Record: {
		// a little bit complicated this one.
		CXCursor c = clang_getTypeDeclaration(type);
		MarshalData data = {out, prefix, uppercasedPrefix};
		fprintf(out, "\tif (!cJSON_IsObject(src))\n\t\treturn %s_ERROR_CONFLICTING_TYPES;\n", uppercasedPrefix);
		fprintf(out, "\tcJSON *item = NULL;\n");
		fprintf(out, "\tenum %s_status status = %s_OK;\n", prefix, uppercasedPrefix);
		clang_visitChildren(c, collectFields, (void *)&data);
		break;
	}
	default:
		break;
	}
	if (!suppressReturn)
		fprintf(out, "\treturn %s_OK;\n", uppercasedPrefix);
	fprintf(out, "}\n\n");

	free(unmarshalerName);
	clang_disposeString(typeSpelling);
	free(uppercasedPrefix);
}
