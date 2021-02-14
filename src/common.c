#include "./common.h"
#include "./jsonstructs.unmarshaler.h"
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

static enum CXChildVisitResult getFirstChildVisitor(CXCursor c, CXCursor parent, CXClientData clientData) {
	CXCursor *cp = clientData;
	*cp = c;
	return CXChildVisit_Break;
}

CXCursor getFirstChild(CXCursor c) {
	CXCursor child = {0};
	clang_visitChildren(c, getFirstChildVisitor, &child);
	return child;
}

char *quoteString(const char *s) {
	cJSON *json = cJSON_CreateStringReference(s);
	char *quoted = cJSON_Print(json);
	cJSON_Delete(json);
	return quoted;
}
