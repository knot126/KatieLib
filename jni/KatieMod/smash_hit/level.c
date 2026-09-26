/**
 * Do stuff in levels
 */

#include "../common/lua_utils.h"
#include "smashhit.h"
#include "../util.h"

#define GET_PLAYER_ID(L, idx) ((!lua_isnoneornil(L, idx)) ? lua_tointeger(L, idx) : -1)

void (*Level_hitSomething)(Level*, int);

int knLevelHitSomething(lua_State *script) {
	if (!Level_hitSomething) Level_hitSomething = YipLookupSymbol("_ZN5Level12hitSomethingEi");
	Level_hitSomething(gGame->level, GET_PLAYER_ID(script, 1));
	return 0;
}

void (*Level_streakAbort)(Level*, int);

int knLevelStreakAbort(lua_State *script) {
	if (!Level_streakAbort) Level_streakAbort = YipLookupSymbol("_ZN5Level11streakAbortEi");
	Level_streakAbort(gGame->level, GET_PLAYER_ID(script, 1));
	return 0;
}

void (*Level_streakInc)(Level*, int);

int knLevelStreakInc(lua_State *script) {
	if (!Level_streakInc) Level_streakInc = YipLookupSymbol("_ZN5Level9streakIncEi");
	Level_streakInc(gGame->level, GET_PLAYER_ID(script, 1));
	return 0;
}

void (*Level_addScore)(Level*, int, int);

int knLevelAddScore(lua_State *script) {
	if (!Level_addScore) Level_addScore = YipLookupSymbol("_ZN5Level8addScoreEii");
	Level_addScore(gGame->level, lua_tointeger(script, 1), GET_PLAYER_ID(script, 2));
	return 0;
}

void (*Level_shoot)(Level *this, float x, float y, float force, bool param_5, int player_id);

int knShoot(lua_State *L) {
	if (!Level_shoot) {
		Level_shoot = YipLookupSymbol("_ZN5Level5shootE6QiVec2fbi");
	}
	
	float x = lua_tonumber(L, 1);
	float y = lua_tonumber(L, 2);
	float force = lua_tonumber(L, 3);
	bool idk = lua_tonumber(L, 4);
	int player_id = GET_PLAYER_ID(L, 5);
	
	Level_shoot(gGame->level, x, y, force, idk, player_id);
	
	return 0;
}

int knEnableLevel(lua_State *script) {
	knRegisterFunc(script, knLevelHitSomething);
	knRegisterFunc(script, knLevelStreakAbort);
	knRegisterFunc(script, knLevelStreakInc);
	knRegisterFunc(script, knLevelAddScore);
	knRegisterFunc(script, knShoot);
	
	return 0;
}
