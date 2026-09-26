#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct StringSet {
	uint32_t length;
	uint32_t capacity;
	char **elements;
} StringSet;

uint32_t StringSet_add(StringSet *self, const char *str) {
	// printf("add(%s)\n", str);
	
	if (self->length >= self->capacity) {
		uint32_t new_cap = 2 * self->capacity + 1;
		char **new_elems = realloc(self->elements, sizeof *self->elements * new_cap);
		
		if (new_elems) {
			self->capacity = new_cap;
			self->elements = new_elems;
		}
		else {
			return 1;
		}
	}
	
	char *new_str = strdup(str);
	
	if (!new_str) {
		return 1;
	}
	
	if (self->length == 0) {
		self->elements[0] = new_str;
		self->length++;
		
		return 0;
	}
	else {
		// Find correct index to insert at, assuming the entire list is sorted.
		uint32_t index;
		uint32_t left = 0, right = self->length;
		
		while (1) {
			const uint32_t midpoint = (left + right) / 2;
			const int32_t compared = strcmp(new_str, self->elements[midpoint]);
			
			if (compared < 0) {
				// new str goes before elem[midpoint]
				right = midpoint;
			}
			else if (compared == 0) {
				// str matches elem[midpoint]
				free(new_str);
				return 0;
			}
			else {
				// new str goes after elem[midpoint]
				left = midpoint + 1;
			}
			
			if (left == right) {
				index = left;
				break;
			}
			else if (left > right) {
				fprintf(stderr, "left > right (%u > %u): something is wrong, abort!\n", left, right);
				abort();
			}
		}
		
		// This just inserts it
		// printf("Insert at %d\n", index);
		
		memmove(&self->elements[index + 1], &self->elements[index], (self->length - index) * sizeof *self->elements);
		self->elements[index] = new_str;
		self->length++;
		
		return 0;
	}
}

typedef void (StringSetPredicate)(void *context, const char *string);

void StringSet_forEach(StringSet *self, void *context, StringSetPredicate predicate) {
	for (uint32_t i = 0; i < self->length; i++) {
		predicate(context, self->elements[i]);
	}
}

void StringSet_initWithElement(StringSet *self, const char * const *array, uint32_t length) {
	
}

void StringSet_initWithElements(StringSet *self, const char * const *array, uint32_t length) {
	memset(self, 0, sizeof *self);
	
	for (uint32_t i = 0; i < length; i++) {
		StringSet_add(self, array[i]);
	}
}

#ifdef STRING_SET_TEST
void testPredicate(void *context, const char *string) {
	printf("Entry: %s\n", string);
}

int main(int argc, const char *argv[]) {
	StringSet set;
	StringSet_initWithElements(&set, argv + 1, argc - 1);
	StringSet_forEach(&set, NULL, testPredicate);
	return 0;
}
#endif
