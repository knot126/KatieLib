/**
 * Registries: register custom code with the game
 */

#include "string_set.c"

#include "registry.h"

StringSet registries[KN_NUM_REGISTRIES];

const char *KNInitRegistry(void) {
	StringSet_add(KN_LEVEL_SCRIPT_TICKERS, "tick");
	StringSet_add(KN_LEVEL_SCRIPT_INITERS, "init");
	StringSet_add(KN_SCRIPT_INIT_FUNCS, "init");
	StringSet_add(KN_SCRIPT_FRAME_FUNCS, "frame");
	StringSet_add(KN_SCRIPT_DRAW_FUNCS, "draw");
	StringSet_add(KN_SCRIPT_DRAW_WORLD_FUNCS, "drawWorld");
	StringSet_add(KN_SCRIPT_COMMAND_HANDLERS, "handleCommand");
	StringSet_add(KN_POWERUPS, "tick");
}
