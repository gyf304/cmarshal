#include "./common.h"

#include "./allocation.h"
#include "./marshal.h"
#include "./finders.h"
#include "./config.h"

#define MAX_TYPES_COUNT 4096

typedef struct {
	CXType type;
	CMarshalTypeAnnotation anno;
} AnnotatedType;

static int typeExists(CXType type, AllocationContext *ctx) {
	int found = 0;
	// mucho inefficient but meh.
	for (int i = 0; i < ctx->size; i++) {
		AnnotatedType *existingType = (AnnotatedType *)ctx->pointers[i];
		if (clang_equalTypes(existingType->type, type)) {
			found = 1;
			break;
		}
	}
	return found;
}

static void typeCallback(void *context, CXType type, CMarshalTypeAnnotation *anno) {
	AllocationContext *ctx = context;
	if (typeExists(type, ctx)) {
		return;
	}
	AnnotatedType *atype = allocateFromContext(ctx, sizeof(AnnotatedType));
	atype->type = type;
	atype->anno.marshal = 0;
	atype->anno.unmarshal = 0;
	if (anno)
		atype->anno = *anno;
}

int main(int argc, char *argv[])
{
	int status = 0;
	if (argc < 2) {
		return -1;
	}

	int randNum = rand();

	/* pulling something really janky here. kids, don't be like me */
	argv[0] = "-fparse-all-comments";
	const char *inputFileName = argv[argc-1];

	CXIndex index = clang_createIndex(0, 0);
	CXTranslationUnit unit = clang_parseTranslationUnit(
		index,
		inputFileName, (void *)argv, argc-1,
		NULL, 0,
		CXTranslationUnit_None
	);
	if (unit == NULL)
	{
		fprintf(stderr, "Unable to parse translation unit. Quitting.\n");
		return -1;
	}

	CXCursor cursor = clang_getTranslationUnitCursor(unit);

	CMarshalConfig config = {0};
	config.cJSONInclude = strdup("cJSON.h");
	config.marshalerPrefix = strdup("marshal");
	config.unmarshalerPrefix = strdup("unmarshal");

	if (!findConfig(cursor, configCallback, &config)) {
		fprintf(stderr, "Config not found in translation unit, exiting.\n");
		fprintf(stderr, "Hint: Check config validity, also try appending a \";\" to the config comment.\n");

		freeConfigFields(&config);
		clang_disposeTranslationUnit(unit);
		clang_disposeIndex(index);
		return -1;
	}

	/* collect types */
	AllocationContext *annotatedTypes = createAllocationContext(0);

	AllocationContext *marshalerTypes = createAllocationContext(0);
	AllocationContext *unmarshalerTypes = createAllocationContext(0);
	findAnnotatedTypes(cursor, typeCallback, annotatedTypes);

	for (int i = 0; i < annotatedTypes->size; i++) {
		AnnotatedType *atype = (AnnotatedType *)(annotatedTypes->pointers[i]);
		if (atype->anno.marshal) {
			AnnotatedType *atypeCopy = allocateFromContext(marshalerTypes, sizeof(AnnotatedType));
			*atypeCopy = *atype;
		}

		if (atype->anno.unmarshal) {
			AnnotatedType *atypeCopy = allocateFromContext(unmarshalerTypes, sizeof(AnnotatedType));
			*atypeCopy = *atype;
		}
	}

	destroyAllocationContext(annotatedTypes);

	/* unfold types */
	for (int i = 0; i < marshalerTypes->size; i++) {
		AnnotatedType *atype = (AnnotatedType *)(marshalerTypes->pointers[i]);
		findDependentTypes(atype->type, typeCallback, marshalerTypes);
	}

	for (int i = 0; i < unmarshalerTypes->size; i++) {
		AnnotatedType *atype = (AnnotatedType *)(unmarshalerTypes->pointers[i]);
		findDependentTypes(atype->type, typeCallback, unmarshalerTypes);
	}

	for (int unmarshal = 0; status == 0 && unmarshal < 2; unmarshal++) {
		enum GenerateStatus genstat = GENERATE_OK;
		char *headerPath = NULL;
		char *implPath = NULL;
		AllocationContext *types = NULL;
		if (unmarshal == 0) {
			headerPath = config.marshalerHeaderFile;
			implPath = config.marshalerImplFile;
			types = marshalerTypes;
		} else {
			headerPath = config.unmarshalerHeaderFile;
			implPath = config.unmarshalerImplFile;
			types = unmarshalerTypes;
		}

		if (headerPath == NULL)
			continue;

		FILE *headerFile = fopen(headerPath, "w");
		if (headerFile == NULL) {
			fprintf(stderr, "Cannot open %s\n", headerPath);
			status = -1;
			break;
		}
		FILE *implFile = implPath ? fopen(implPath, "w") : NULL;
		if (implPath != NULL && implFile == NULL) {
			fclose(headerFile);
			fprintf(stderr, "Cannot open %s\n", implPath);
			status = -1;
			break;
		}
		/* write headers */
		fprintf(headerFile, "/* auto-generated, do not edit */\n");
		fprintf(headerFile, "#ifndef _CMARSHAL_%s_%d\n", unmarshal ? "UNMARSHALER" : "MARSHALER", randNum);
		fprintf(headerFile, "#define _CMARSHAL_%s_%d\n", unmarshal ? "UNMARSHALER" : "MARSHALER", randNum);
		fprintf(headerFile, "#include \"%s\"\n", config.cJSONInclude);
		fprintf(headerFile, "#include \"%s\"\n", inputFileName);
		genHeaderPreamble(headerFile, &config, unmarshal);
		if (implFile) {
			fprintf(implFile, "/* auto-generated, do not edit */\n");
			fprintf(implFile, "#include \"%s\"\n", headerPath);
			fprintf(implFile, "#include <string.h>\n");
		}
		/* first generate forward declarations */
		for (int i = 0; status == 0 && i < types->size; i++) {
			AnnotatedType *atype = (AnnotatedType *)(types->pointers[i]);
			int export = unmarshal ? atype->anno.unmarshal : atype->anno.marshal;
			genstat = genForwardDecl(export ? headerFile : implFile, &config, atype->type, export ? NULL : "static", unmarshal);
			if (genstat != GENERATE_OK) {
				status = -1;
				break;
			}
		}
		/* then generate implementations */
		for (int i = 0; status == 0 && i < types->size; i++) {
			AnnotatedType *atype = (AnnotatedType *)(types->pointers[i]);
			int export = unmarshal ? atype->anno.unmarshal : atype->anno.marshal;
			genstat = genImpl(implFile, &config, atype->type, export ? NULL : "static", unmarshal);
			if (genstat != GENERATE_OK) {
				status = -1;
				break;
			}
		}
		if (implFile)
			fclose(implFile);
		fprintf(headerFile, "#endif\n");
		fclose(headerFile);
	}

	destroyAllocationContext(marshalerTypes);
	destroyAllocationContext(unmarshalerTypes);

	freeConfigFields(&config);

	clang_disposeTranslationUnit(unit);
	clang_disposeIndex(index);

	return status;
}
