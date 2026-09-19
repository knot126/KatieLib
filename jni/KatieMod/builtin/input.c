/**
 * Get touchscreen input
 */

#include <math.h>
#include <android/native_activity.h>
#include "../util.h"
#include "lua_utils.h"

#define gInput (gGame->input)

int (*QiInput_getTouchCount)(QiInput *this);
bool (*QiInput_hasTouch)(QiInput *this, int index);
int (*QiInput_getTouchPosX)(QiInput *this, int index);
int (*QiInput_getTouchPosY)(QiInput *this, int index);
bool (*QiInput_wasTouchPressed)(QiInput *this, int index);
bool (*QiInput_wasTouchReleased)(QiInput *this, int index);
bool (*QiInput_isKeyDown)(QiInput *this, int key);
bool (*QiInput_wasKeyPressed)(QiInput *this, int key);
bool (*QiInput_wasKeyReleased)(QiInput *this, int key);

void (*QiInput_registerKeyDown)(QiInput *this, int key);
void (*QiInput_registerKeyUp)(QiInput *this, int key);

int knGetTouchCount(lua_State *L) {
	lua_pushinteger(L, QiInput_getTouchCount(gInput));
	return 1;
}

int knHasTouch(lua_State *L) {
	lua_pushboolean(L, QiInput_hasTouch(gInput, lua_tointeger(L, 1)));
	return 1;
}

int knGetTouchPos(lua_State *L) {
	int i = lua_tointeger(L, 1);
	int x = QiInput_getTouchPosX(gInput, i);
	int y = QiInput_getTouchPosY(gInput, i);
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	return 2;
}

int knWasTouchPressed(lua_State *L) {
	lua_pushboolean(L, QiInput_wasTouchPressed(gInput, lua_tointeger(L, 1)));
	return 1;
}

int knWasTouchReleased(lua_State *L) {
	lua_pushboolean(L, QiInput_wasTouchReleased(gInput, lua_tointeger(L, 1)));
	return 1;
}

int knGetKey(lua_State *L, int index) {
	const int t = lua_type(L, index);
	
	switch (t) {
		case LUA_TNUMBER: {
			return lua_tointeger(L, index);
		}
		case LUA_TSTRING: {
			const char *s = lua_tostring(L, index);
			return s[0];
		}
		default: {
			return 0;
		}
	}
}

int knIsKeyDown(lua_State *L) {
	lua_pushboolean(L, QiInput_isKeyDown(gInput, knGetKey(L, 1)));
	return 1;
}

int knWasKeyPressed(lua_State *L) {
	lua_pushboolean(L, QiInput_wasKeyPressed(gInput, knGetKey(L, 1)));
	return 1;
}

int knWasKeyReleased(lua_State *L) {
	lua_pushboolean(L, QiInput_wasKeyReleased(gInput, knGetKey(L, 1)));
	return 1;
}

// Simulated input
int knRegisterKeyDown(lua_State *L) {
	QiInput_registerKeyDown(gInput, knGetKey(L, 1));
	return 0;
}

int knRegisterKeyUp(lua_State *L) {
	QiInput_registerKeyUp(gInput, knGetKey(L, 1));
	return 0;
}

/**
 * Keyboard implementation
 */
int32_t (*onInputEvent)(struct android_app* app, AInputEvent* event);

static inline int mapKeyToChar(int32_t keyCode, int32_t meta) {
	const bool shift = (meta & AMETA_SHIFT_ON);
	
	if (keyCode >= AKEYCODE_0 && keyCode <= AKEYCODE_9) {
		return '0' + (keyCode - AKEYCODE_0);
	}
	else if (keyCode == AKEYCODE_STAR)                       { return '*'; }
	else if (keyCode == AKEYCODE_POUND)                      { return '#'; }
	else if (keyCode >= AKEYCODE_A && keyCode <= AKEYCODE_Z) {
		return (shift ? 'A' : 'a') + (keyCode - AKEYCODE_A);
	}
	else if (keyCode == AKEYCODE_COMMA)                      { return ','; }
	else if (keyCode == AKEYCODE_PERIOD)                     { return '.'; }
	else if (keyCode == AKEYCODE_SPACE)                      { return ' '; }
	else if (keyCode == AKEYCODE_GRAVE)                      { return '`'; }
	else if (keyCode == AKEYCODE_MINUS)                      { return '-'; }
	else if (keyCode == AKEYCODE_EQUALS)                     { return '='; }
	else if (keyCode == AKEYCODE_LEFT_BRACKET)               { return '['; }
	else if (keyCode == AKEYCODE_RIGHT_BRACKET)              { return ']'; }
	else if (keyCode == AKEYCODE_BACKSLASH)                  { return '\\'; }
	else if (keyCode == AKEYCODE_SEMICOLON)                  { return ';'; }
	else if (keyCode == AKEYCODE_APOSTROPHE)                 { return '\''; }
	else if (keyCode == AKEYCODE_SLASH)                      { return '/'; }
	else if (keyCode == AKEYCODE_AT)                         { return '@'; }
	else if (keyCode == AKEYCODE_MOVE_END)                   { return 0x10d; }
	else if (keyCode == AKEYCODE_MOVE_HOME)                  { return 0x10c; }
	else if (keyCode == AKEYCODE_DEL)                        { return 0x101; } // backspace???
	else if (keyCode == AKEYCODE_FORWARD_DEL)                { return 0x102; }
	else if (keyCode == AKEYCODE_DPAD_LEFT)                  { return 0x109; }
	else if (keyCode == AKEYCODE_DPAD_RIGHT)                 { return 0x10a; }
	else if (keyCode == AKEYCODE_ESCAPE)                     { return 0x100; } // not really known but closes dev menu
	else if (keyCode == AKEYCODE_TAB)                        { return 0x103; } // not actually known
	else if (keyCode == AKEYCODE_CTRL_LEFT || keyCode == AKEYCODE_CTRL_RIGHT) {
		return 0x10b;
	}
	else {
		return 0;
	}
}

static int32_t onInputEventHook(struct android_app* app, AInputEvent* event) {
	QiInput *gAndroidInput = YipLookupSymbol("gAndroidInput");
	
	const uint32_t source = AInputEvent_getSource(event);
	const uint32_t type = AInputEvent_getType(event);
	
	if (type == AINPUT_EVENT_TYPE_KEY && source == AINPUT_SOURCE_KEYBOARD) {
		const int32_t keycode = AKeyEvent_getKeyCode(event);
		const int32_t action = AKeyEvent_getAction(event);
		const int32_t meta = AKeyEvent_getMetaState(event);
		const int key = mapKeyToChar(keycode, meta);
		
		if (!key) {
			return 1;
		}
		
		switch (action) {
			case AKEY_EVENT_ACTION_DOWN: {
				QiInput_registerKeyDown(gAndroidInput, key);
				break;
			}
			case AKEY_EVENT_ACTION_UP: {
				QiInput_registerKeyUp(gAndroidInput, key);
				break;
			}
		}
		
		return 1;
	}
	else {
		return onInputEvent(app, event);
	}
}

int knEnableInput(lua_State *L) {
	// Get raw input
	knRegisterFunc(L, knGetTouchCount);
	knRegisterFunc(L, knHasTouch);
	knRegisterFunc(L, knGetTouchPos);
	knRegisterFunc(L, knWasTouchPressed);
	knRegisterFunc(L, knWasTouchReleased);
	knRegisterFunc(L, knIsKeyDown);
	knRegisterFunc(L, knWasKeyPressed);
	knRegisterFunc(L, knWasKeyReleased);
	
	// Simulated input
	knRegisterFunc(L, knRegisterKeyDown);
	knRegisterFunc(L, knRegisterKeyUp);
	
	// Functions from QiInput we need
	QiInput_getTouchCount = YipLookupSymbol("_ZNK7QiInput13getTouchCountEv");
	QiInput_hasTouch = YipLookupSymbol("_ZNK7QiInput8hasTouchEi");
	QiInput_getTouchPosX = YipLookupSymbol("_ZNK7QiInput12getTouchPosXEi");
	QiInput_getTouchPosY = YipLookupSymbol("_ZNK7QiInput12getTouchPosYEi");
	QiInput_wasTouchPressed = YipLookupSymbol("_ZNK7QiInput15wasTouchPressedEi");
	QiInput_wasTouchReleased = YipLookupSymbol("_ZNK7QiInput16wasTouchReleasedEi");
	QiInput_registerKeyDown = YipLookupSymbol("_ZN7QiInput15registerKeyDownEi");
	QiInput_registerKeyUp = YipLookupSymbol("_ZN7QiInput13registerKeyUpEi");
	QiInput_isKeyDown = YipLookupSymbol("_ZNK7QiInput9isKeyDownEi");
	QiInput_wasKeyPressed = YipLookupSymbol("_ZNK7QiInput13wasKeyPressedEi");
	QiInput_wasKeyReleased = YipLookupSymbol("_ZNK7QiInput14wasKeyReleasedEi");
	
	return 0;
}

const char *KNInitKeyboard(void) {
	// arm64 only for now
	onInputEvent = YipHookFunctionAt(0x45b68, onInputEventHook, false);
	return NULL;
}
