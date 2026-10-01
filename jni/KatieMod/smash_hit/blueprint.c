#include <yiploader/yiploader.h>
#include <stdbool.h>

#include "../util.h"
#include "../common/lua_utils.h"
#include "smashhit.h"

typedef struct StringList {
	const char **elements;
	uint32_t length;
	uint32_t capacity;
} StringList;

int32_t StringList_insert(StringList *self, const char *element, uint32_t index) {
	if (index > self->length) {
		return -3;
	}
	
	if (self->capacity <= self->length) {
		uint32_t newCapacity = self->capacity + (self->capacity >> 1) + 1;
		const char **newData = realloc(self->elements, newCapacity * sizeof *self->elements);
		
		if (newData) {
			self->capacity = newCapacity;
			self->elements = newData;
		}
		else {
			return -1;
		}
	}
	
	element = strdup(element);
	
	if (!element) {
		return -2;
	}
	
	const uint32_t moveCount = self->length - index;
	
	if (moveCount > 0) {
		memmove(&self->elements[index + 1], &self->elements[index], moveCount * sizeof *self->elements);
	}
	
	self->elements[index] = element;
	
	return 0;
}

const char *StringList_index(StringList *self, uint32_t index) {
	if (index < self->length) {
		return self->elements[index];
	}
	else {
		return NULL;
	}
}

void StringList_free(StringList *self) {
	for (uint32_t i = 0; i < self->length; i++) {
		free((char *) self->elements[i]);
	}
}

void StringList_init(StringList *self, const char *firstElement) {
	memset(self, 0, sizeof *self->elements);
	StringList_insert(self, firstElement, 0);
}

static int createStringList(lua_State *L, const char *variable, const char *first) {
	StringList *st = lua_createuserdata(L, sizeof *st);
	lua_setglobal(L, variable);
	
	return 0;
}

void Script_draw_hook(Script *this) {
	if (this->active) {
		
	}
}

const char *KNInitBlueprint(void) {
	YipHookFunction("_ZN6Script4drawEv", Script_draw_hook, true);
	
	return NULL;
}
