/**
 * Registries: register custom code with the game
 */

#ifndef _KATIE_REGISTRY_H_
#define _KATIE_REGISTRY_H_

enum {
	KN_LEVEL_SCRIPT_TICK_FUNCS,
	KN_SCRIPT_DRAW_FUNCS,
	KN_SCRIPT_DRAW_WORLD_FUNCS,
	KN_SCRIPT_HANDLE_COMMAND_FUNCS,
	
	KN_NUM_REGISTRIES,
};

void Registry_add(int32_t id, const char *key);
const char *Registry_index(int32_t id, uint32_t index);

#endif
