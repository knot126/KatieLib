/**
 * Registries: register custom code with the game
 */

#ifndef _KATIE_REGISTRY_H_
#define _KATIE_REGISTRY_H_

enum {
	KN_LEVEL_SCRIPT_INIT_FUNCS = 0,
	KN_LEVEL_SCRIPT_TICK_FUNCS,
	KN_SCRIPT_INIT_FUNCS,
	KN_SCRIPT_FRAME_FUNCS,
	KN_SCRIPT_DRAW_FUNCS,
	KN_SCRIPT_DRAW_WORLD_FUNCS,
	KN_SCRIPT_HANDLE_COMMAND_FUNCS,
	
	KN_NUM_REGISTRIES,
};

typedef void (*RegistryIterator)(void *context, const char *value);

#endif
