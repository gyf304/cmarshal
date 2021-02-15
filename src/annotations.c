#include "./annotations.h"

#include "./allocation.h"

static cJSON *getAnnotation(CXCursor c)
{
	CXString comment = clang_Cursor_getRawCommentText(c);
	const char *commentStr = clang_getCString(comment);
	const char *bracketStartStr = "cmarshal:`";
	if (commentStr) {
		CXString kindSpelling = clang_getCursorKindSpelling(c.kind);
		// fprintf(stderr, "COMMENT @ %s: %s\n", clang_getCString(kindSpelling), commentStr);
		clang_disposeString(kindSpelling);
		char *lBracket = strstr(commentStr, bracketStartStr);
		if (lBracket) {
			char *lBracketEnd = lBracket + strlen(bracketStartStr);
			char *rBracket = strstr(lBracketEnd, "`");
			if (rBracket) {
				return cJSON_ParseWithLength(lBracketEnd, rBracket-lBracketEnd);
			}
		}
	}
	clang_disposeString(comment);
	return NULL;
}

static void *unmarshal_allocator(void *context, size_t size) {
	AllocationContext *ctx = context;
	return allocateFromContext(ctx, size);
}

CMarshalConfig *getCMarshalConfig(CXCursor c) {
	if (c.kind != CXCursor_UnexposedDecl) {
		return NULL;
	}
	cJSON *json = getAnnotation(c);
	if (json == NULL) {
		return NULL;
	}
	void *buf = calloc(sizeof(CMarshalConfig) + sizeof(void *), 1);
	CMarshalConfig *anno = buf;
	AllocationContext **pctx = buf + sizeof(CMarshalConfig);
	*pctx = createAllocationContext(32);
	enum unmarshal_status status = unmarshal_CMarshalConfig(anno, json, unmarshal_allocator, *pctx);
	if (status != UNMARSHAL_OK) {
		destroyAllocationContext(*pctx);
		free(buf);
		anno = NULL;
	}
	cJSON_Delete(json);
	return anno;
}

void freeCMarshalConfig(CMarshalConfig *anno) {
	void *buf = (void *)anno;
	AllocationContext **pctx = buf + sizeof(CMarshalConfig);
	destroyAllocationContext(*pctx);
	free(buf);
}

CMarshalMemberAnnotation *getCMarshalMemberAnnotation(CXCursor c) {
	if (c.kind != CXCursor_FieldDecl) {
		return NULL;
	}
	cJSON *json = getAnnotation(c);
	if (json == NULL) {
		return NULL;
	}
	void *buf = calloc(sizeof(CMarshalMemberAnnotation) + sizeof(void *), 1);
	CMarshalMemberAnnotation *anno = buf;
	AllocationContext **pctx = buf + sizeof(CMarshalMemberAnnotation);
	*pctx = createAllocationContext(32);
	enum unmarshal_status status = unmarshal_CMarshalMemberAnnotation(anno, json, unmarshal_allocator, *pctx);
	if (status != UNMARSHAL_OK) {
		destroyAllocationContext(*pctx);
		free(buf);
		anno = NULL;
	}
	cJSON_Delete(json);
	return anno;
}

void freeCMarshalMemberAnnotation(CMarshalMemberAnnotation *anno) {
	void *buf = (void *)anno;
	AllocationContext **pctx = buf + sizeof(CMarshalMemberAnnotation);
	destroyAllocationContext(*pctx);
	free(buf);
}

CMarshalTypeAnnotation *getCMarshalTypeAnnotation(CXType t) {
	CXCursor c = clang_getTypeDeclaration(t);
	cJSON *json = getAnnotation(c);
	if (json == NULL) {
		return NULL;
	}
	void *buf = calloc(sizeof(CMarshalTypeAnnotation) + sizeof(void *), 1);
	CMarshalTypeAnnotation *anno = buf;
	AllocationContext **pctx = buf + sizeof(CMarshalTypeAnnotation);
	*pctx = createAllocationContext(32);
	enum unmarshal_status status = unmarshal_CMarshalTypeAnnotation(anno, json, unmarshal_allocator, *pctx);
	if (status != UNMARSHAL_OK) {
		if (cJSON_IsTrue(json)) {
			/* special casing for cmarshal:`true` short hand */
			// anno->enabled = true;
			anno->marshal = true;
			anno->unmarshal = true;
		} else {
			destroyAllocationContext(*pctx);
			free(buf);
			anno = NULL;
		}
	}
	cJSON_Delete(json);
	return anno;
}

void freeCMarshalTypeAnnotation(CMarshalTypeAnnotation *anno) {
	void *buf = (void *)anno;
	AllocationContext **pctx = buf + sizeof(CMarshalTypeAnnotation);
	destroyAllocationContext(*pctx);
	free(buf);
}

