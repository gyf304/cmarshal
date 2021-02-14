#ifndef _CONFIG_H
#define _CONFIG_H

#include "common.h"

static void configCallback(void *context, CMarshalConfig *config) {
	CMarshalConfig *target = (CMarshalConfig *) context;
	if (config->cJSONInclude) {
		free(target->cJSONInclude);
		target->cJSONInclude = strdup(config->cJSONInclude);
	}
	if (config->marshalerHeaderFile) {
		free(target->marshalerHeaderFile);
		target->marshalerHeaderFile = strdup(config->marshalerHeaderFile);
	}
	if (config->marshalerImplFile) {
		free(target->marshalerImplFile);
		target->marshalerImplFile = strdup(config->marshalerImplFile);
	}
	if (config->marshalerPrefix) {
		free(target->marshalerPrefix);
		target->marshalerPrefix = strdup(config->marshalerPrefix);
	}
	if (config->unmarshalerHeaderFile) {
		free(target->unmarshalerHeaderFile);
		target->unmarshalerHeaderFile = strdup(config->unmarshalerHeaderFile);
	}
	if (config->unmarshalerImplFile) {
		free(target->unmarshalerImplFile);
		target->unmarshalerImplFile = strdup(config->unmarshalerImplFile);
	}
}

static void freeConfigFields(CMarshalConfig *config) {
	free(config->cJSONInclude);
	free(config->marshalerHeaderFile);
	free(config->marshalerImplFile);
	free(config->marshalerPrefix);
	free(config->unmarshalerHeaderFile);
	free(config->unmarshalerImplFile);
	free(config->unmarshalerPrefix);
}

#endif
