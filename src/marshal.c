#include "./marshal.h"

typedef struct {
	FILE* out;
	const char *prefix;
} MarshalData;

static enum CXChildVisitResult collectFields(CXCursor c, CXCursor parent, CXClientData client_data) {
	/* c must point to a record (struct or union) */
	MarshalData *config = client_data;
	const char *prefix = config->prefix;
	FILE* out = config->out;

	CXType type = clang_getCursorType(c);
	CXString name = clang_getCursorSpelling(c);
	const char *nameStr = clang_getCString(name);
	cJSON *keyJSON = cJSON_CreateString(nameStr);

	char *marshalerName = getPrefixedEscapedTypeName(prefix, type);
	char *lenField = NULL;
	cJSON *annotation = getAnnotation(c);

	cJSON *cmarshal = cJSON_GetObjectItemCaseSensitive(annotation, "cmarshal");
	if (cJSON_IsObject(cmarshal)) {
		cJSON *lenItem = cJSON_GetObjectItemCaseSensitive(cmarshal, "length");
		if (cJSON_IsString(lenItem)) {
			lenField = cJSON_GetStringValue(lenItem);
		}
		cJSON *keyItem = cJSON_GetObjectItemCaseSensitive(cmarshal, "key");
		if (cJSON_IsString(keyItem)) {
			cJSON_Delete(keyJSON);
			keyJSON = keyItem;
		}
		if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(cmarshal, "ignore"))) {
			cJSON_Delete(keyJSON);
			keyJSON = NULL;
		}
	}

	if (keyJSON) {
		char *keyLiteral = cJSON_Print(keyJSON);
		cJSON_Delete(keyJSON);
		if (lenField) { // array-like
			CXType pointeeType = clang_getPointeeType(type);
			char *pointeeMarshalerName = getPrefixedEscapedTypeName(prefix, pointeeType);
			fprintf(out, "\t{ /* field %s */\n", nameStr);
			fprintf(out, "\t\tcJSON* arr = cJSON_CreateArray();\n");
			fprintf(out, "\t\tfor (int i = 0; i < (int)(src->%s); i++) {\n", lenField);
			fprintf(out, "\t\t\tcJSON_AddItemToArray(arr, %s(&src->%s[i]));\n", pointeeMarshalerName, nameStr);
			fprintf(out, "\t\t}\n");
			fprintf(out, "\t\tcJSON_AddItemToObject(obj, %s, arr);\n", keyLiteral);
			fprintf(out, "\t}\n");
			free(pointeeMarshalerName);
			free(lenField);
		} else {
			fprintf(out, "\tcJSON_AddItemToObject(obj, %s, %s(&src->%s));\n", keyLiteral, marshalerName, nameStr);
		}
		free(keyLiteral);
	}

	free(marshalerName);
	return CXChildVisit_Continue;
}

/*
 * C code generators
 */
void genMarshalerForwardDecl(CXType type, FILE *out, const char *prefix, const char *specifiers)
{
	CXString typeSpelling = clang_getTypeSpelling(type);
	CXCursor declCursor = clang_getTypeDeclaration(type);
	const char *typeSpellingStr = clang_getCString(typeSpelling);
	char *marshalerName = getPrefixedEscapedTypeName(prefix, type);

	fprintf(
		out,
		"/* marshaler for type %s */\n",
		typeSpellingStr
	);
	fprintf(
		out,
		"%s%scJSON* %s(%s* src);\n\n",
		specifiers ? specifiers : "",
		specifiers ? " " : "",
		marshalerName,
		typeSpellingStr
	);

	free(marshalerName);
	clang_disposeString(typeSpelling);
}

void genMarshalerImpl(CXType type, FILE *out, const char *prefix, const char *specifiers)
{
	CXString typeSpelling = clang_getTypeSpelling(type);
	CXCursor declCursor = clang_getTypeDeclaration(type);
	const char *typeSpellingStr = clang_getCString(typeSpelling);
	char *marshalerName = getPrefixedEscapedTypeName(prefix, type);

	// generate marshaler
	fprintf(
		out,
		"%s%scJSON* %s(%s* src) \n{\n",
		specifiers ? specifiers : "",
		specifiers ? " " : "",
		marshalerName,
		typeSpellingStr
	);

	switch (type.kind)
	{
	case CXType_Bool:
		fprintf(out, "\treturn cJSON_CreateBool(*src);\n");
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
		fprintf(out, "\treturn cJSON_CreateNumber((double) *src);\n");
		break;
	case CXType_Pointer: {
		CXType pointeeType = clang_getPointeeType(type);
		if (pointeeType.kind == CXType_Char_S || pointeeType.kind == CXType_Char_U) {
			fprintf(out, "\tif (*src == NULL) {\n\t\treturn cJSON_CreateNull();\n\t}\n");
			fprintf(out, "\treturn cJSON_CreateString(*src);\n");
		} else {
			char *marshalerName = getPrefixedEscapedTypeName(prefix, pointeeType);
			fprintf(out, "\tif (*src == NULL) {\n\t\treturn cJSON_CreateNull();\n\t}\n");
			fprintf(out, "\treturn %s(*src);\n", marshalerName);
			free(marshalerName);
		}
		break;
	}
	case CXType_Typedef: {
		CXCursor child = getFirstChild(clang_getTypeDeclaration(type));
		CXType actualType = clang_getCursorType(child);
		char *marshalerName = getPrefixedEscapedTypeName(prefix, actualType);
		fprintf(out, "\treturn %s((void *) src);\n", marshalerName);
		free(marshalerName);
		break;
	}
	case CXType_Record: {
		// a little bit complicated this one.
		CXCursor c = clang_getTypeDeclaration(type);
		MarshalData data = {out, prefix};
		fprintf(out, "\tcJSON* obj = cJSON_CreateObject();\n");
		clang_visitChildren(c, collectFields, (void *)&data);
		fprintf(out, "\treturn obj;\n");
		break;
	}
	default:
		break;
	}

	// ending quote
	fprintf(out, "}\n\n");

	free(marshalerName);
	clang_disposeString(typeSpelling);
}
