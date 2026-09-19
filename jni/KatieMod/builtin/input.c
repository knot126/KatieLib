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
void (*QiInput_registerMousePos)(QiInput *this, int x, int y);
void (*QiInput_registerButtonDown)(QiInput *this, int button);
void (*QiInput_registerButtonUp)(QiInput *this, int button);

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
	const bool shift = (meta & AMETA_SHIFT_ON) == AMETA_SHIFT_ON;
	
	if (keyCode >= AKEYCODE_0 && keyCode <= AKEYCODE_9) {
		return '0' + (keyCode - AKEYCODE_0);
	}
	else if (keyCode == AKEYCODE_STAR)                       { return '*'; }
	else if (keyCode == AKEYCODE_POUND)                      { return '#'; }
	else if (keyCode >= AKEYCODE_A && keyCode <= AKEYCODE_Z) {
		return (shift ? 'A' : 'a') + (keyCode - AKEYCODE_A);
	}
	else if (keyCode == AKEYCODE_COMMA)                      { return shift ? '<' : ','; }
	else if (keyCode == AKEYCODE_PERIOD)                     { return shift ? '>' : '.'; }
	else if (keyCode == AKEYCODE_SPACE)                      { return ' '; }
	else if (keyCode == AKEYCODE_GRAVE)                      { return shift ? '~' : '`'; }
	else if (keyCode == AKEYCODE_MINUS)                      { return shift ? '_' : '-'; }
	else if (keyCode == AKEYCODE_EQUALS)                     { return shift ? '+' : '='; }
	else if (keyCode == AKEYCODE_LEFT_BRACKET)               { return shift ? '{' : '['; }
	else if (keyCode == AKEYCODE_RIGHT_BRACKET)              { return shift ? '}' : ']'; }
	else if (keyCode == AKEYCODE_BACKSLASH)                  { return shift ? '|' : '\\'; }
	else if (keyCode == AKEYCODE_SEMICOLON)                  { return shift ? ':' : ';'; }
	else if (keyCode == AKEYCODE_APOSTROPHE)                 { return shift ? '\"' : '\''; }
	else if (keyCode == AKEYCODE_SLASH)                      { return shift ? '?' : '/'; }
	else if (keyCode == AKEYCODE_AT)                         { return '@'; }
	else if (keyCode == AKEYCODE_ESCAPE)                     { return 0x100; } // not really known but closes dev menu
	else if (keyCode == AKEYCODE_DEL)                        { return 0x101; } // backspace???
	else if (keyCode == AKEYCODE_FORWARD_DEL)                { return 0x102; }
	else if (keyCode == AKEYCODE_TAB)                        { return 0x103; } // not actually known
	else if (keyCode == AKEYCODE_DPAD_UP)                    { return 0x107; } // guess
	else if (keyCode == AKEYCODE_DPAD_DOWN)                  { return 0x108; } // guess
	else if (keyCode == AKEYCODE_DPAD_LEFT)                  { return 0x109; }
	else if (keyCode == AKEYCODE_DPAD_RIGHT)                 { return 0x10a; }
	else if (keyCode == AKEYCODE_MOVE_HOME)                  { return 0x10c; }
	else if (keyCode == AKEYCODE_MOVE_END)                   { return 0x10d; }
	else if (keyCode == AKEYCODE_CTRL_LEFT || keyCode == AKEYCODE_CTRL_RIGHT) {
		return 0x10b;
	}
	else {
		return 0;
	}
}

static int32_t onInputEventHook(struct android_app* app, AInputEvent* event) {
	static QiInput *gAndroidInput;
	
	if (!gAndroidInput) { gAndroidInput = YipLookupSymbol("gAndroidInput"); }
	
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
	else if (type == AINPUT_EVENT_TYPE_MOTION && source == AINPUT_SOURCE_MOUSE) {
		const int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
		const int32_t pointer_index = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> 8;
		// const int32_t action_button = AMotionEvent_getActionButton(event);
		const int32_t x = (int32_t) AMotionEvent_getX(event, pointer_index);
		const int32_t y = (int32_t) AMotionEvent_getY(event, pointer_index);
		
		switch (action) {
			case AMOTION_EVENT_ACTION_DOWN: {
				QiInput_registerButtonDown(gAndroidInput, 1);
				QiInput_registerMousePos(gAndroidInput, x, y);
				break;
			}
			case AMOTION_EVENT_ACTION_CANCEL:
			case AMOTION_EVENT_ACTION_UP: {
				QiInput_registerButtonUp(gAndroidInput, 1);
				QiInput_registerMousePos(gAndroidInput, x, y);
				break;
			}
			case AMOTION_EVENT_ACTION_MOVE: {
				QiInput_registerMousePos(gAndroidInput, x, y);
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
	QiInput_registerMousePos = YipLookupSymbol("_ZN7QiInput16registerMousePosEii");
	QiInput_registerButtonDown = YipLookupSymbol("_ZN7QiInput18registerButtonDownEi");
	QiInput_registerButtonUp = YipLookupSymbol("_ZN7QiInput16registerButtonUpEi");
	
	return 0;
}

#define INPUT_EVENT_HANDLER_ADDRESS 0x45b68

const char *KNInitKeyboard(void) {
	// we should use YipGetAndroidAppStruct()->onInputEvent in the future...
	onInputEvent = YipHookFunctionAt(INPUT_EVENT_HANDLER_ADDRESS, onInputEventHook, false);
	return NULL;
}
