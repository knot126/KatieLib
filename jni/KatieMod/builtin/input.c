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
int (*QiInput_getMousePosX)(QiInput *this);
int (*QiInput_getMousePosY)(QiInput *this);

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

int knGetMousePos(lua_State *L) {
	int x = QiInput_getMousePosX(gInput);
	int y = QiInput_getMousePosY(gInput);
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	return 2;
}

int knCaptureMouse(lua_State *L) {
	KNCaptureMouse(lua_toboolean(L, 1));
	return 0;
}

struct MouseDelta {
	int x, y;
} gMouseDelta;

int knGetMouseDelta(lua_State *L) {
	lua_pushinteger(L, gMouseDelta.x);
	lua_pushinteger(L, gMouseDelta.y);
	gMouseDelta.x = 0;
	gMouseDelta.y = 0;
	return 2;
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
enum {
	KN_KEY_ESCAPE             = 0x100, // not really known but closes dev menu
	KN_KEY_BACKSPACE          = 0x101,
	KN_KEY_DELETE             = 0x102,
	KN_KEY_TAB                = 0x103, // not actually known, toggles dev menu
	KN_KEY_ALT                = 0x104, // made up for katielib
	KN_KEY_SHIFT              = 0x105, // made up for katielib
	KN_KEY_META               = 0x106, // made up for katielib
	KN_KEY_UP_ARROW           = 0x107, // guess
	KN_KEY_DOWN_ARROW         = 0x108, // guess
	KN_KEY_LEFT_ARROW         = 0x109,
	KN_KEY_RIGHT_ARROW        = 0x10a,
	KN_KEY_CONTROL            = 0x10b,
	KN_KEY_HOME               = 0x10c,
	KN_KEY_END                = 0x10d,
	KN_KEY_MENU               = 0x10e, // made up for katielib
};

int32_t (*onInputEvent)(struct android_app* app, AInputEvent* event);

static inline int mapKeyToChar(int32_t keyCode, int32_t meta) {
	// const bool shift = (meta & AMETA_SHIFT_ON) == AMETA_SHIFT_ON;
	
	if (keyCode >= AKEYCODE_0 && keyCode <= AKEYCODE_9) {
		return '0' + (keyCode - AKEYCODE_0);
	}
	else if (keyCode == AKEYCODE_STAR)                       { return '*'; }
	else if (keyCode == AKEYCODE_POUND)                      { return '#'; }
	else if (keyCode >= AKEYCODE_A && keyCode <= AKEYCODE_Z) {
		return 'a' + (keyCode - AKEYCODE_A);
	}
	else if (keyCode == AKEYCODE_COMMA)                      { return ','; }
	else if (keyCode == AKEYCODE_PERIOD)                     { return '.'; }
	else if (keyCode == AKEYCODE_ALT_LEFT || keyCode == AKEYCODE_ALT_RIGHT) {
		return KN_KEY_ALT;
	}
	else if (keyCode == AKEYCODE_SHIFT_LEFT || keyCode == AKEYCODE_SHIFT_RIGHT) {
		return KN_KEY_SHIFT;
	}
	else if (keyCode == AKEYCODE_TAB)                        { return KN_KEY_TAB; }
	else if (keyCode == AKEYCODE_SPACE)                      { return ' '; }
	else if (keyCode == AKEYCODE_ENTER)                      { return '\n'; }
	else if (keyCode == AKEYCODE_DEL)                        { return KN_KEY_BACKSPACE; }
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
	else if (keyCode == AKEYCODE_PLUS)                       { return '+'; }
	else if (keyCode == AKEYCODE_MENU)                       { return KN_KEY_MENU; }
	else if (keyCode == AKEYCODE_ESCAPE)                     { return KN_KEY_ESCAPE; } 
	else if (keyCode == AKEYCODE_FORWARD_DEL)                { return KN_KEY_DELETE; }
	else if (keyCode == AKEYCODE_CTRL_LEFT || keyCode == AKEYCODE_CTRL_RIGHT) {
		return KN_KEY_CONTROL;
	}
	else if (keyCode == AKEYCODE_META_LEFT || keyCode == AKEYCODE_META_RIGHT) {
		return KN_KEY_META;
	}
	else if (keyCode == AKEYCODE_DPAD_UP)                    { return KN_KEY_UP_ARROW; }
	else if (keyCode == AKEYCODE_DPAD_DOWN)                  { return KN_KEY_DOWN_ARROW; }
	else if (keyCode == AKEYCODE_DPAD_LEFT)                  { return KN_KEY_LEFT_ARROW; }
	else if (keyCode == AKEYCODE_DPAD_RIGHT)                 { return KN_KEY_RIGHT_ARROW; }
	else if (keyCode == AKEYCODE_MOVE_HOME)                  { return KN_KEY_HOME; }
	else if (keyCode == AKEYCODE_MOVE_END)                   { return KN_KEY_END; }
	else if (keyCode >= AKEYCODE_NUMPAD_0 && keyCode <= AKEYCODE_NUMPAD_9) {
		return '0' + (keyCode - AKEYCODE_NUMPAD_0);
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
// #if 0
	else if (type == AINPUT_EVENT_TYPE_MOTION &&
		(source == AINPUT_SOURCE_MOUSE || source == AINPUT_SOURCE_MOUSE_RELATIVE)) {
		const int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
		const int32_t pointer_index = (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> 8;
		// const int32_t action_button = AMotionEvent_getActionButton(event);
		const int32_t x = (int32_t) AMotionEvent_getX(event, pointer_index);
		const int32_t y = (int32_t) AMotionEvent_getY(event, pointer_index);
		
		// HACK: This is a hack for more reliable mouse deltas for e.g. cameras
		// and stuff.
		gMouseDelta.x += x;
		gMouseDelta.y += y;
		
		switch (action) {
			case AMOTION_EVENT_ACTION_DOWN:
			// case AMOTION_EVENT_ACTION_BUTTON_PRESS:
			case AMOTION_EVENT_ACTION_POINTER_DOWN: {
				QiInput_registerButtonDown(gAndroidInput, 1);
				QiInput_registerMousePos(gAndroidInput, x, y);
				break;
			}
			case AMOTION_EVENT_ACTION_CANCEL:
			case AMOTION_EVENT_ACTION_UP:
			// case AMOTION_EVENT_ACTION_BUTTON_RELEASE:
			case AMOTION_EVENT_ACTION_POINTER_UP: {
				QiInput_registerButtonUp(gAndroidInput, 1);
				QiInput_registerMousePos(gAndroidInput, x, y);
				break;
			}
			case AMOTION_EVENT_ACTION_MOVE:
			case AMOTION_EVENT_ACTION_HOVER_MOVE:
			case AMOTION_EVENT_ACTION_HOVER_ENTER:
			case AMOTION_EVENT_ACTION_HOVER_EXIT:
			case AMOTION_EVENT_ACTION_OUTSIDE:
			case AMOTION_EVENT_ACTION_SCROLL: {
				QiInput_registerMousePos(gAndroidInput, x, y);
				break;
			}
			default: {
				// QiInput_registerButtonUp(gAndroidInput, 1);
				QiInput_registerMousePos(gAndroidInput, x, y);
			}
		}
		
		return 1;
	}
// #endif
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
	knRegisterFunc(L, knGetMousePos);
	knRegisterFunc(L, knCaptureMouse);
	knRegisterFunc(L, knGetMouseDelta);
	
	// Simulated input
	knRegisterFunc(L, knRegisterKeyDown);
	knRegisterFunc(L, knRegisterKeyUp);
	
	// Key codes
	knLuaPushEnum(L, KN_KEY_ESCAPE);
	knLuaPushEnum(L, KN_KEY_BACKSPACE);
	knLuaPushEnum(L, KN_KEY_DELETE);
	knLuaPushEnum(L, KN_KEY_TAB);
	knLuaPushEnum(L, KN_KEY_ALT);
	knLuaPushEnum(L, KN_KEY_SHIFT);
	knLuaPushEnum(L, KN_KEY_META);
	knLuaPushEnum(L, KN_KEY_UP_ARROW);
	knLuaPushEnum(L, KN_KEY_DOWN_ARROW);
	knLuaPushEnum(L, KN_KEY_LEFT_ARROW);
	knLuaPushEnum(L, KN_KEY_RIGHT_ARROW);
	knLuaPushEnum(L, KN_KEY_CONTROL);
	knLuaPushEnum(L, KN_KEY_HOME);
	knLuaPushEnum(L, KN_KEY_END);
	knLuaPushEnum(L, KN_KEY_MENU);
	
	return 0;
}

#define INPUT_EVENT_HANDLER_ADDRESS 0x45b68

const char *KNInitKeyboard(void) {
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
	QiInput_getMousePosX = YipLookupSymbol("_ZNK7QiInput12getMousePosXEv");
	QiInput_getMousePosY = YipLookupSymbol("_ZNK7QiInput12getMousePosYEv");
	
#ifdef INPUT_EVENT_HANDLER_ADDRESS
	onInputEvent = YipHookFunctionAt(INPUT_EVENT_HANDLER_ADDRESS, onInputEventHook, false);
#else
	onInputEvent = YipHookFunctionPointer(YipGetAndroidAppStruct()->onInputEvent, onInputEventHook, false);
#endif
	return NULL;
}
