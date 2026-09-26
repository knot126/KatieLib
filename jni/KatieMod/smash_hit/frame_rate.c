/**
 * Frame rate adjustment
 */

#include <math.h>
#include "../common/lua_utils.h"
#include "smashhit.h"
#include "../util.h"

int knGetDeviceHz(lua_State *script) {
	/**
	 * (float) hz = knGetDeviceHz()
	 * 
	 * Get the native refresh rate for the default display.
	 */
	
	lua_pushnumber(script, KNGetRefreshRate());
	return 1;
}

#define ROUND(x) (floor(x * 10000.0f) / 10000.0f)

int knSetFrameRate(lua_State *script) {
	/**
	 * (bool) success = knSetFrameRate([(float) frameRate, [(int) sleepTime]])
	 * 
	 * Set the game framerate and adjust the physics time step to match. Smash
	 * Hit normally runs at 60hz, even if the device has a higher refresh rate
	 * display. This will make adjustments so that the game runs at a higher
	 * refresh rate, if available.
	 * 
	 * Calling knSetFrameRate() with no arguments will set the game to match the
	 * refresh rate of the device. This is recommended if you're going to change
	 * the refresh rate at all.
	 * 
	 * Only available on 64-bit ARM devices.
	 */
	
#if defined(__aarch64__)
	int argc = lua_gettop(script);
	
	float targetFPS;
	
	if (argc > 0) {
		targetFPS = lua_tonumber(script, 1);
	}
	else {
		targetFPS = KNGetRefreshRate();
	}
	
	float timeStep = 1.0 / targetFPS;
	float sleepTime = ROUND(timeStep);
	
	if (argc > 1) {
		sleepTime = lua_tonumber(script, 2);
	}
	
	float *v1 = (YipLookupSymbol("_ZN4GameC2EP6Deviceii") + 0xd68);
	float *v2 = (YipLookupSymbol("_ZN4Game6updateEv") + 0x300);
	float *v3 = (YipLookupSymbol("android_main") + 0xd9c);
	
	*v1 = timeStep;
	*v2 = timeStep;
	*v3 = sleepTime;
	
	lua_pushboolean(script, true);
#else
	lua_pushboolean(script, false);
#endif
	return 1;
}

int knEnableFramerate(lua_State *script) {
	knRegisterFunc(script, knGetDeviceHz);
	knRegisterFunc(script, knSetFrameRate);
	
	return 0;
}
