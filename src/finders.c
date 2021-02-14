#include "./finders.h"

#include "./annotations.h"

typedef struct {
	TypeCallback *cb;
	void *ctx;
} FindTypesVisitorClientData;

typedef struct {
	ConfigCallback *cb;
	void *ctx;
	int found;
} FindConfigVisitorClientData;

static int isTypedefOfNamelessRecord(CXType type) {
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

static enum CXChildVisitResult findAnnotatedTypesVisitor(CXCursor c, CXCursor parent, CXClientData clientData)
{
	FindTypesVisitorClientData *data = clientData;
	enum CXCursorKind kind = clang_getCursorKind(c);
	if (c.kind != CXCursor_TypedefDecl && c.kind != CXCursor_StructDecl && c.kind != CXCursor_UnionDecl) {
		return CXChildVisit_Recurse;
	}
	CXType type = clang_getCursorType(c);
	if (isTypedefOfNamelessRecord(type)) {
		return CXChildVisit_Recurse;
	}
	CMarshalTypeAnnotation *anno = getCMarshalTypeAnnotation(type);
	if (anno) {
		data->cb(data->ctx, type, anno);
		freeCMarshalTypeAnnotation(anno);
	}
	return CXChildVisit_Recurse;
}

void findAnnotatedTypes(CXCursor c, TypeCallback *cb, void *context) {
	FindTypesVisitorClientData data = {cb, context};
	clang_visitChildren(c, findAnnotatedTypesVisitor, &data);
}

static enum CXChildVisitResult findDependentTypesVisitor(CXCursor c, CXCursor parent, CXClientData clientData)
{
	FindTypesVisitorClientData *data = clientData;
	enum CXCursorKind kind = clang_getCursorKind(c);
	CXType type = clang_getCursorType(c);
	if (type.kind == CXType_Invalid || isTypedefOfNamelessRecord(type)) {
		return CXChildVisit_Recurse;
	}
	CMarshalTypeAnnotation *anno = getCMarshalTypeAnnotation(type);
	data->cb(data->ctx, type, anno);
	if (anno) {
		freeCMarshalTypeAnnotation(anno);
	}
	return CXChildVisit_Recurse;
}

void findDependentTypes(CXType type, TypeCallback *cb, void *context) {
	FindTypesVisitorClientData data = {cb, context};
	clang_visitChildren(clang_getTypeDeclaration(type), findDependentTypesVisitor, &data);
}

static enum CXChildVisitResult findConfigVisitor(CXCursor c, CXCursor parent, CXClientData clientData)
{
	enum CXChildVisitResult result = CXChildVisit_Recurse;
	FindConfigVisitorClientData *data = clientData;
	if (data != NULL) {
		CMarshalConfig *anno = getCMarshalConfig(c);
		data->cb(data->ctx, anno);
		freeCMarshalConfig(anno);
		data->found = 1;
		return CXChildVisit_Break;
	}
	return CXChildVisit_Recurse;
}

int findConfig(CXCursor c, ConfigCallback *cb, void *context) {
	FindConfigVisitorClientData data = {cb, context, 0};
	clang_visitChildren(c, findConfigVisitor, &data);
	return data.found;
}
