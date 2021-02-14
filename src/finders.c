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

static enum CXChildVisitResult findAnnotatedTypesVisitor(CXCursor c, CXCursor parent, CXClientData clientData)
{
	FindTypesVisitorClientData *data = clientData;
	if (c.kind != CXCursor_TypedefDecl && c.kind != CXCursor_StructDecl && c.kind != CXCursor_UnionDecl) {
		return CXChildVisit_Recurse;
	}
	CXType type = clang_getCursorType(c);
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
	CXType type = clang_getCursorType(c);

	if (type.kind != CXType_Invalid) {
		CMarshalTypeAnnotation *anno = getCMarshalTypeAnnotation(type);
		data->cb(data->ctx, type, anno);
		if (anno) {
			freeCMarshalTypeAnnotation(anno);
		}
	}

	return CXChildVisit_Recurse;
}

void findDependentTypes(CXType type, TypeCallback *cb, void *context) {
	FindTypesVisitorClientData data = {cb, context};
	switch (type.kind) {
	case CXType_Typedef: {
		CXType typedefedType = clang_getTypedefDeclUnderlyingType(clang_getTypeDeclaration(type));
		if (typedefedType.kind != CXType_Invalid && typedefedType.kind != CXType_Elaborated) {
			cb(context, typedefedType, context);
		}
	}
	default:
		clang_visitChildren(clang_getTypeDeclaration(type), findDependentTypesVisitor, &data);
		break;
	}
}

static enum CXChildVisitResult findConfigVisitor(CXCursor c, CXCursor parent, CXClientData clientData)
{
	FindConfigVisitorClientData *data = clientData;
	if (c.kind != CXCursor_UnexposedDecl) {
		return CXChildVisit_Continue;
	}
	if (data != NULL) {
		CMarshalConfig *anno = getCMarshalConfig(c);
		if (anno != NULL) {
			data->cb(data->ctx, anno);
			freeCMarshalConfig(anno);
			data->found = 1;
			return CXChildVisit_Break;
		}
	}
	return CXChildVisit_Continue;
}

int findConfig(CXCursor c, ConfigCallback *cb, void *context) {
	FindConfigVisitorClientData data = {cb, context, 0};
	clang_visitChildren(c, findConfigVisitor, &data);
	return data.found;
}
