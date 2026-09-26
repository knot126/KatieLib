/**
 * Legacy game control functions that will be removed at some point
 */

#include <math.h>

#include "../common/lua_utils.h"

#include "../util.h"
#include "smashhit.h"

/**
 * NO CLIP
 */
shortop_t gNoclipBufferedInstruction = KN_RET;

#define NOCLIP_IS_ON (gNoclipBufferedInstruction != KN_RET)

void swap_noclip_state(void) {
	shortop_t *hitSomething = YipLookupSymbol("_ZN5Level12hitSomethingEi");
	shortop_t currentInstr = hitSomething[0];
	hitSomething[0] = gNoclipBufferedInstruction;
	gNoclipBufferedInstruction = currentInstr;
}

int knSetNoclip(lua_State *script) {
	/**
	 * Set noclip as enabled or disabled.
	 */
	
	// Swap if states don't match
	if ((NOCLIP_IS_ON) != lua_toboolean(script, 1)) {
		swap_noclip_state();
	}
	
	return 0;
}

int knGetNoclip(lua_State *script) {
	/**
	 * Get the current noclip state.
	 */
	
	lua_pushboolean(script, NOCLIP_IS_ON);
	return 1;
}

int knEnableGamectl(lua_State *script) {
	// Cheats
	knRegisterFunc(script, knSetNoclip);
	knRegisterFunc(script, knGetNoclip);
	
	return 0;
}
