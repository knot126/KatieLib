/**
 * Player modification
 * 
 * This is for modifying stuff in the Player struct. For anything related to
 * gameplay, see level.c
 */

#include "../common/lua_utils.h"
#include "smashhit.h"
#include "../util.h"

#define GET_PLAYER_ID(L, I) (lua_isnoneornil(L, I) ? (-1) : lua_tointeger(L, I))

int knSetBalls(lua_State *script) {
	/**
	 * Set the player's ball count
	 */
	
	int pid = GET_PLAYER_ID(script, 2);
	
	if (pid < 0) {
		gGame->player->balls = lua_tointeger(script, 1);
	}
	else {
		gGame->player->mpBalls[pid] = lua_tointeger(script, 1);
	}
	
	return 0;
}

int knGetBalls(lua_State *script) {
	/**
	 * Get the player's ballcount. This is accurate even if knSetBalls was used.
	 */
	
	int pid = GET_PLAYER_ID(script, 1);
	
	if (pid < 0) {
		lua_pushinteger(script, gGame->player->balls);
	}
	else {
		lua_pushinteger(script, gGame->player->mpBalls[pid]);
	}
	
	return 1;
}

int knSetStreak(lua_State *script) {
	/**
	 * Set the player's streak
	 */
	
	int pid = GET_PLAYER_ID(script, 2);
	
	if (pid < 0) {
		gGame->player->streak = lua_tointeger(script, 1);
	}
	else {
		gGame->player->mpStreak[pid] = lua_tointeger(script, 1);
	}
	
	return 0;
}

int knGetStreak(lua_State *script) {
	/**
	 * Get the player's streak. This is accurate even if knSetStreak was used.
	 */
	
	int pid = GET_PLAYER_ID(script, 1);
	
	if (pid < 0) {
		lua_pushinteger(script, gGame->player->streak);
	}
	else {
		lua_pushinteger(script, gGame->player->mpStreak[pid]);
	}
	
	return 1;
}

int knEnablePlayer(lua_State *script) {
	knRegisterFunc(script, knSetBalls);
	knRegisterFunc(script, knGetBalls);
	knRegisterFunc(script, knSetStreak);
	knRegisterFunc(script, knGetStreak);
	
	return 0;
}
