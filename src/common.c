#include "./common.h"
#include "./jsonstructs.gen.h"
#include "./allocation.h"

#include <ctype.h>

static char *escapeTypeName(const char *src)
{
	char *escaped = malloc(strlen(src) + 1);
	strcpy(escaped, src);

	for (char *p = escaped; *p != '\0'; p++) {
		if (*p == '*') {
			*p = 'p';
		} else if (!isalnum(*p)) {
			*p = '_';
		}
	}

	return escaped;
}

char *getPrefixedEscapedTypeName(const char *prefix, CXType type)
{
	CXString typeSpelling = clang_getTypeSpelling(type);
	char *escapedTypeSpellingStr = escapeTypeName(clang_getCString(typeSpelling));
	clang_disposeString(typeSpelling);

	char *name = malloc(strlen(prefix) + strlen(escapedTypeSpellingStr) + 2);
	sprintf(name, "%s_%s", prefix, escapedTypeSpellingStr);

	free(escapedTypeSpellingStr);
	return name;
}

cJSON *getAnnotation(CXCursor c)
{
	CXString comment = clang_Cursor_getRawCommentText(c);
	const char *commentStr = clang_getCString(comment);
	const char *bracketStartStr = "cmarshal:`";
	if (commentStr) {
		CXString kindSpelling = clang_getCursorKindSpelling(c.kind);
		fprintf(stderr, "COMMMENT @ %s: %s\n", clang_getCString(kindSpelling), commentStr);
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

static enum CXChildVisitResult getFirstChildVisitor(CXCursor c, CXCursor parent, CXClientData client_data) {
	CXCursor *cp = client_data;
	*cp = c;
	return CXChildVisit_Break;
}

CXCursor getFirstChild(CXCursor c) {
	CXCursor child = {0};
	clang_visitChildren(c, getFirstChildVisitor, &child);
	return child;
}

int isTypedefOfNamelessRecord(CXType type) {
	if (type.kind != CXType_Typedef) {
		return 0;
	}
	int nameless = 0;
	CXCursor child = getFirstChild(clang_getTypeDeclaration(type));
	CXType actualType = clang_getCursorType(child);
	if (actualType.kind == CXType_Record) {
		CXString s = clang_getCursorSpelling(child);
		if (strlen(clang_getCString(s)) == 0) {
			nameless = 1;
		}
		clang_disposeString(s);
	}

	return nameless;
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
	if (c.kind != CXCursor_UnexposedDecl) {
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

char *quoteString(const char *s) {
	cJSON *json = cJSON_CreateStringReference(s);
	char *quoted = cJSON_Print(json);
	cJSON_Delete(json);
	return quoted;
}
