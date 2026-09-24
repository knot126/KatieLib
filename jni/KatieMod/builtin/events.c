#include "../util.h"
#include "lua_utils.h"
#include <yiploader/yiploader.h>
#include <stdbool.h>

void (*Game_frame)(Game *this);
void (*Level_handleInput)(Level *this, QiInput *inputSource);

void send_event_noret_noarg(Game *this, const char *function_name) {
	if (this->hudScene && *this->hudScene->script.state) {
		knLuaCallByName(*this->hudScene->script.state, function_name);
	}
	else if (this->menuScene && *this->menuScene->script.state) {
		knLuaCallByName(*this->menuScene->script.state, function_name);
	}
}

bool send_event_cont_noarg(Game *this, const char *function_name) {
	if (this->hudScene && *this->hudScene->script.state) {
		return knLuaCallByNameBool(*this->hudScene->script.state, function_name);
	}
	else if (this->menuScene && *this->menuScene->script.state) {
		return knLuaCallByNameBool(*this->menuScene->script.state, function_name);
	}
	else {
		return false;
	}
}

void Game_frame_hook(Game *this) {
	send_event_noret_noarg(this, "onFrameStart");
	Game_frame(this);
	send_event_noret_noarg(this, "onFrameEnd");
}

void Level_handleInput_hook(Level *this, QiInput *inputSource) {
	bool override = send_event_cont_noarg(gGame, "onHandleInput");
	
	if (!override) {
		Level_handleInput(this, inputSource);
	}
}

const char *KNInitEvents(void) {
	Game_frame = YipHookFunction("_ZN4Game5frameEv", Game_frame_hook, false);
	Level_handleInput = YipHookFunction("_ZN5Level11handleInputERK7QiInput", Level_handleInput_hook, false);
	return NULL;
}
