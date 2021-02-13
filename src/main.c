#include "./common.h"

#include "./marshal.h"
#include "./unmarshal.h"

#define MAX_TYPES_COUNT 4096

CXType types[MAX_TYPES_COUNT] = {0};
int typesCount = 0;

const char *args[] = {
	"-fparse-all-comments"
};

static int typeExists(CXType type) {
	int found = 0;
	// mucho inefficient but meh.
	for (int i = 0; i < typesCount; i++) {
		if (clang_equalTypes(types[i], type)) {
			found = 1;
			break;
		}
	}
	return found;
}

static enum CXChildVisitResult expander(CXCursor c, CXCursor parent, CXClientData client_data) {
	CXType type = clang_getCursorType(c);
	CXString typeSpelling = clang_getTypeSpelling(type);
	if (typesCount < MAX_TYPES_COUNT) {
		if (!typeExists(type) && !isTypedefOfNamelessRecord(type)) {
			types[typesCount++] = type;
		}
	}
	clang_disposeString(typeSpelling);
	return CXChildVisit_Continue;
}

// we scan the AST for `{"cmarshal": true}` comments
static enum CXChildVisitResult visitor(CXCursor c, CXCursor parent, CXClientData client_data)
{
	enum CXCursorKind kind = clang_getCursorKind(c);
	// CXCursor_TypedefDecl
	CXType type = clang_getCursorType(c);
	if (type.kind == CXType_Invalid) {
		return CXChildVisit_Recurse;
	}
	cJSON *annotation = getAnnotation(c);
	if (annotation && typesCount < MAX_TYPES_COUNT) {
		if (cJSON_IsTrue(annotation) && !typeExists(type) && !isTypedefOfNamelessRecord(type)) {
			types[typesCount++] = type;
		}
	}
	cJSON_Delete(annotation);
	return CXChildVisit_Recurse;
}

static enum CXChildVisitResult findConfig(CXCursor c, CXCursor parent, CXClientData client_data)
{
	enum CXChildVisitResult result = CXChildVisit_Recurse;
	CMarshalConfig **annop = client_data;
	CMarshalConfig *anno = getCMarshalConfig(c);
	if (anno != NULL) {
		*annop = anno;
		return CXChildVisit_Break;
	}
	return CXChildVisit_Recurse;
}

static void expandTypes()
{
	for (int i = 0; i < typesCount; i++) {
		CXCursor c = clang_getTypeDeclaration(types[i]);
		clang_visitChildren(c, expander, NULL);
	}
}

int main(int argc, const char *argv[])
{
	if (argc < 2) {
		return -1;
	}
	const char *preamble = "/* Auto-generated, do not edit */";

	CXIndex index = clang_createIndex(0, 0);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		argv[1], args, sizeof(args) / sizeof(args[0]),
		NULL, 0,
		CXTranslationUnit_None
	);
	if (unit == NULL)
	{
		fprintf(stderr, "Unable to parse translation unit. Quitting.\n");
		return -1;
	}

	CXCursor cursor = clang_getTranslationUnitCursor(unit);

	CMarshalConfig *config = NULL;
	clang_visitChildren(cursor, findConfig, &config);
	if (config == NULL) {
		fprintf(stderr, "Cannot find configuration, quitting\n");
		return -1;
	}

	// fill in defaults. This is fine because we track allocations out-of-band.
	if (config->cJSONInclude == NULL) {
		config->cJSONInclude = "cJSON.h";
	}
	if (config->marshalerPrefix == NULL) {
		config->marshalerPrefix = "marshal";
	}
	if (config->unmarshalerPrefix == NULL) {
		config->unmarshalerPrefix = "unmarshal";
	}

	clang_visitChildren(cursor, visitor, NULL);

	int exportedTypesCount = typesCount;
	expandTypes();

	if (config->marshalerHeaderFile != NULL) {
		FILE *out = fopen(config->marshalerHeaderFile, "w");
		if (out != NULL) {
			fprintf(out, "%s\n", preamble);
			fprintf(out, "#include \"%s\"\n", config->cJSONInclude);
			fprintf(out, "#include \"%s\"\n\n", argv[1]);
			fprintf(out, "/*\n * Exported Marshaler Function Declarations \n */\n");
			for (int i = 0; i < exportedTypesCount; i++) {
				genMarshalerForwardDecl(types[i], out, config->marshalerPrefix, NULL);
			}
			fclose(out);
		}
	}

	if (config->marshalerHeaderFile != NULL && config->marshalerImplFile != NULL) {
		FILE *out = fopen(config->marshalerImplFile, "w");
		if (out != NULL) {
			fprintf(out, "%s\n", preamble);
			fprintf(out, "#include \"%s\"\n", config->marshalerHeaderFile);
			fprintf(out, "/*\n * Unexported Marshaler Function Declarations \n */\n");
			for (int i = exportedTypesCount; i < typesCount; i++) {
				genMarshalerForwardDecl(types[i], out, config->marshalerPrefix, "static");
			}
			fprintf(out, "/*\n * Marshaler Function Implementations \n */ \n");
			for (int i = 0; i < typesCount; i++) {
				const char *specifier = NULL;
				if (i >= exportedTypesCount)
					specifier = "static";
				genMarshalerImpl(types[i], out, config->marshalerPrefix, specifier);
			}
			fclose(out);
		}
	}

	if (config->unmarshalerHeaderFile != NULL) {
		FILE *out = fopen(config->unmarshalerHeaderFile, "w");
		if (out != NULL) {
			fprintf(out, "%s\n", preamble);
			fprintf(out, "#include \"%s\"\n", config->cJSONInclude);
			fprintf(out, "#include \"%s\"\n\n", argv[1]);
			genUnmarshalUtils(out, config->unmarshalerPrefix);
			fprintf(out, "/*\n * Exported Unmarshaler Function Declarations \n */\n");
			for (int i = 0; i < exportedTypesCount; i++) {
				genUnmarshalerForwardDecl(types[i], out, config->unmarshalerPrefix, NULL);
			}
			fclose(out);
		}
	}

	if (config->unmarshalerHeaderFile != NULL && config->unmarshalerImplFile != NULL) {
		FILE *out = fopen(config->unmarshalerImplFile, "w");
		if (out != NULL) {
			fprintf(out, "%s\n", preamble);
			fprintf(out, "#include \"%s\"\n", config->unmarshalerHeaderFile);
			genUnmarshalerImplPreamble(out, config->unmarshalerPrefix);
			fprintf(out, "/*\n * Unexported Unmarshaler Function Declarations \n */\n");
			for (int i = exportedTypesCount; i < typesCount; i++) {
				genUnmarshalerForwardDecl(types[i], out, config->unmarshalerPrefix, "static");
			}
			fprintf(out, "/*\n * Unmarshaler Function Implementations \n */ \n");
			for (int i = 0; i < typesCount; i++) {
				const char *specifier = NULL;
				if (i >= exportedTypesCount)
					specifier = "static";
				genUnmarshalerImpl(types[i], out, config->unmarshalerPrefix, specifier);
			}
			fclose(out);
		}
	}

	freeCMarshalConfig(config);
	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);
}
