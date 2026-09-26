/**
 * Player modification
 * 
 * This is for modifying stuff in the Player struct. For anything related to
 * gameplay, see level.c
 */

#include "../common/lua_utils.h"
#include "smashhit.h"
#include "../util.h"

int knSetBalls(lua_State *script) {
	/**
	 * Set the player's ball count
	 */
	
	gGame->player->balls = lua_tointeger(script, 1);
	return 0;
}

int knGetBalls(lua_State *script) {
	/**
	 * Get the player's ballcount. This is accurate even if knSetBalls was used.
	 */
	
	lua_pushinteger(script, gGame->player->balls);
	return 1;
}

int knSetStreak(lua_State *script) {
	/**
	 * Set the player's streak
	 */
	
	gGame->player->streak = lua_tointeger(script, 1);
	return 0;
}

int knGetStreak(lua_State *script) {
	/**
	 * Get the player's streak. This is accurate even if knSetStreak was used.
	 */
	
	lua_pushinteger(script, gGame->player->streak);
	return 1;
}

int knEnablePlayer(lua_State *script) {
	knRegisterFunc(script, knSetBalls);
	knRegisterFunc(script, knGetBalls);
	knRegisterFunc(script, knSetStreak);
	knRegisterFunc(script, knGetStreak);
	
	return 0;
}
