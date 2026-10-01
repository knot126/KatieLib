/**
 * Registries: register custom code with the game
 */

#include "string_set.c"

#include "registry.h"

StringSet registries[KN_NUM_REGISTRIES];

static inline void Registry_init(int32_t id, const char *firstValue) {
	StringSet_initWithElement(registries[id], firstValue);
}

void Registry_add(int32_t id, const char *key) {
	StringSet_add(registries[id], key);
}

const char *Registry_index(int32_t id, uint32_t index) {
	return StringSet_index(registries[id], index);
}

const char *KNInitRegistry(void) {
	Registry_init(KN_LEVEL_SCRIPT_INIT_FUNCS, "init");
	Registry_init(KN_LEVEL_SCRIPT_TICK_FUNCS, "tick");
	Registry_init(KN_SCRIPT_INIT_FUNCS, "init");
	Registry_init(KN_SCRIPT_FRAME_FUNCS, "frame");
	Registry_init(KN_SCRIPT_DRAW_FUNCS, "draw");
	Registry_init(KN_SCRIPT_DRAW_WORLD_FUNCS, "drawWorld");
	Registry_init(KN_SCRIPT_HANDLE_COMMAND_FUNCS, "handleCommand");
	
	return NULL;
}
