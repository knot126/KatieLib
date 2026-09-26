#include "../util.h"
#include "../common/lua_utils.h"
#include <yiploader/yiploader.h>
#include <stdbool.h>

lua_State *getActiveScript(Game *this) {
	if (this->hudScene && *this->hudScene->script.state) {
		return *this->hudScene->script.state;
	}
	else if (this->menuScene && *this->menuScene->script.state) {
		return *this->menuScene->script.state;
	}
	else {
		return NULL;
	}
}

void callEventFunction(Game *this, const char *function_name) {
	lua_State *script = getActiveScript(this);
	
	if (script) {
		knLuaCallVoid(script, function_name, KAT_END);
	}
}

bool callEventFunctionBool(Game *this, const char *function_name) {
	lua_State *script = getActiveScript(this);
	
	if (script) {
		return knLuaCallBool(script, function_name, KAT_END);
	}
	else {
		return false;
	}
}

void (*Game_frame)(Game *this);

void Game_frame_hook(Game *this) {
	callEventFunction(this, "onFrameStart");
	Game_frame(this);
	callEventFunction(this, "onFrameEnd");
}

void (*Level_handleInput)(Level *this, QiInput *inputSource);

void Level_handleInput_hook(Level *this, QiInput *inputSource) {
	bool override = callEventFunctionBool(gGame, "onHandleInput");
	
	if (!override) {
		Level_handleInput(this, inputSource);
	}
}

void (*Level_hitSomething)(Level *this, int playerIndex);

void Level_hitSomething_hook(Level *this, int playerIndex) {
	lua_State *L = getActiveScript(gGame);
	int result = 0;
	
	if (L) {
		result = knLuaCallBool(L, "onHitSomething", KAT_INT, playerIndex, KAT_END);
	}
	
	if (!result) {
		Level_hitSomething(this, playerIndex);
	}
}

void (*Level_streakAbort)(Level *this, int playerIndex);

void Level_streakAbort_hook(Level *this, int playerIndex) {
	lua_State *L = getActiveScript(gGame);
	int result = 0;
	
	if (L) {
		result = knLuaCallBool(L, "onStreakAbort", KAT_INT, playerIndex, KAT_END);
	}
	
	if (!result) {
		Level_streakAbort(this, playerIndex);
	}
}

void (*Level_streakInc)(Level *this, int playerIndex);

void Level_streakInc_hook(Level *this, int playerIndex) {
	lua_State *L = getActiveScript(gGame);
	int result = 0;
	
	if (L) {
		result = knLuaCallBool(L, "onStreakInc", KAT_INT, playerIndex, KAT_END);
	}
	
	if (!result) {
		Level_streakInc(this, playerIndex);
	}
}

void (*Level_enterRoom)(Level *this, Room *room);

void Level_enterRoom_hook(Level *this, Room *room) {
	lua_State *L = getActiveScript(gGame);
	
	if (L) {
		knLuaCallVoid(L, "onEnterRoom", KAT_STR, room->name.data ? room->name.data : room->name.cached, KAT_END);
	}
	
	Level_enterRoom(this, room);
}

const char *KNInitEvents(void) {
	Game_frame = YipHookFunction("_ZN4Game5frameEv", Game_frame_hook, false);
	Level_handleInput = YipHookFunction("_ZN5Level11handleInputERK7QiInput", Level_handleInput_hook, false);
	Level_hitSomething = YipHookFunction("_ZN5Level12hitSomethingEi", Level_hitSomething_hook, false);
	Level_streakAbort = YipHookFunction("_ZN5Level11streakAbortEi", Level_streakAbort_hook, false);
	Level_streakInc = YipHookFunction("_ZN5Level9streakIncEi", Level_streakInc_hook, false);
	Level_enterRoom = YipHookFunction("_ZN5Level9enterRoomEP4Room", Level_enterRoom_hook, false);
	return NULL;
}
