/**
 * Game control (such as setting balls, streak, gamemode etc)
 */

#ifndef GRANNY

#include <dlfcn.h>
#include <math.h>

#include "lua_utils.h"

#include "../util.h"
#include "smashhit.h"

#define MakeQiString(CSTR) (QiString) { \
	.data = (char *) CSTR, \
	.allocated_size = strlen(CSTR), \
	.length = strlen(CSTR), \
}

/**
 * RAW GET/SET BALLS
 */
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

/**
 * LEVEL FUNCTION HELPERS
 */
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

/* Release 16 - this is gone until I can fix the coordinates...
int knLevelExplosion(lua_State *script) {
	Level *level = gGame->level;
	
	QiVec3 pos = {
		.x = lua_tonumber(script, 1),
		.y = lua_tonumber(script, 2),
		.z = -lua_tonumber(script, 3) - level->offsetZ,
	};
	
	float power = lua_tonumber(script, 4);
	
	void (*explosion)(Level*, QiVec3*, float) = YipLookupSymbol("_ZN5Level9explosionERK6QiVec3f");
	explosion(level, &pos, power);
	
	return 0;
}
*/

/**
 * MAIN MENU RELOADING
 * 
 * Support reloading the main menu by simulating a press of the R debug key when
 * reloading is wanted.
 */
bool gWantsReload = false;
bool (*gWasKeyPressedFunc)(QiInput *this, int ch);

bool KNReload_WasKeyPressedHook(QiInput *this, int ch) {
	/**
	 * Simulate pressing the 'R' key if a reload is wanted.
	 */
	
	if (gWantsReload && ch == 'r') {
		gWantsReload = false;
		return true;
	}
	
	return gWasKeyPressedFunc(this, ch);
}

int knReload(lua_State *script) {
	/**
	 * knReload()
	 * 
	 * Reload the main menu or level on the next frame.
	 */
	
	if (!gWasKeyPressedFunc) {
		gWasKeyPressedFunc = YipHookFunction("_ZNK7QiInput13wasKeyPressedEi", KNReload_WasKeyPressedHook, false);
	}
	
	gWantsReload = true;
	return 0;
}

void (*Game_loadTemplates)(Game *this);

int knReloadTemplates(lua_State *script) {
	/**
	 * (bool) success = knReloadTemplates()
	 * 
	 * Reload templates.
	 */
	
	if (!Game_loadTemplates) {
		Game_loadTemplates = YipLookupSymbol("_ZN4Game13loadTemplatesEv");
	}
	
	if (gGame) {
		Game_loadTemplates(gGame);
		lua_pushboolean(script, 1);
	}
	else {
		lua_pushboolean(script, 0);
	}
	
	return 1;
}

void (*Gfx_construct)(Gfx *this, ResMan *resMan);
void (*Gfx_destruct)(Gfx *this);
void (*Gfx_load1)(Gfx *this, ResMan *resMan);
void (*Gfx_load2)(Gfx *this, ResMan *resMan);

int knReloadGfx(lua_State *script) {
	/**
	 * knReloadGfx()
	 * 
	 * Reload all of the game's hardcoded shaders and textures.
	 */
	
	if (!Gfx_load1 || !Gfx_load2) {
		Gfx_construct = YipLookupSymbol("_ZN3GfxC2EP6ResMan");
		Gfx_destruct = YipLookupSymbol("_ZN3GfxD2Ev");
		Gfx_load1 = YipLookupSymbol("_ZN3Gfx5load1EP6ResMan");
		Gfx_load2 = YipLookupSymbol("_ZN3Gfx5load2EP6ResMan");
	}
	
	if (!gGame) {
		return 0;
	}
	
	Gfx_destruct(gGame->gfx);
	Gfx_construct(gGame->gfx, gGame->resman);
	Gfx_load1(gGame->gfx, gGame->resman);
	Gfx_load2(gGame->gfx, gGame->resman);
	
	return 0;
}

void (*Player_load)(Player *this);

int knReloadPlayer(lua_State *script) {
	/**
	 * Reload the save file
	 */
	
	if (!Player_load) {
		Player_load = YipLookupSymbol("_ZN6Player4loadEv");
	}
	
	Player_load(gGame->player);
	
	return 0;
}

// quick save stuff

void (*Player_quickSave)(Player *this);

int knQuickSave(lua_State *script) {
	if (!Player_quickSave) {
		Player_quickSave = YipLookupSymbol("_ZN6Player9quickSaveEv");
	}
	
	Player_quickSave(gGame->player);
	
	return 0;
}

bool (*Player_quickLoad)(Player *this);

int knQuickLoad(lua_State *script) {
	if (!Player_quickLoad) {
		Player_quickLoad = YipLookupSymbol("_ZN6Player9quickLoadEv");
	}
	
	lua_pushboolean(script, Player_quickLoad(gGame->player));
	
	return 1;
}

void (*Player_clearQuickSave)(Player *this);

int knQuickClear(lua_State *script) {
	if (!Player_clearQuickSave) {
		Player_clearQuickSave = YipLookupSymbol("_ZN6Player14clearQuickSaveEv");
	}
	
	Player_clearQuickSave(gGame->player);
	
	return 0;
}

/**
 * Camera shit
 */

QiVec3 cameraPosOffset = {};
QiQuat cameraRotOffset = {};

void (*QiViewport_setCameraPos)(QiViewport *viewport, QiVec3 *pos);
void (*QiViewport_setCameraRot)(QiViewport *viewport, QiQuat *rot);
void (*QiViewport_updateModelview)(QiViewport *self);
QiQuat (*QiQuat_compose)(QiQuat *self, QiQuat *other);

void QiViewport_setCameraPos_hook(QiViewport *viewport, QiVec3 *pos) {
	QiVec3 new_pos;
	new_pos.x = pos->x + cameraPosOffset.x;
	new_pos.y = pos->y + cameraPosOffset.y;
	new_pos.z = pos->z + cameraPosOffset.z;
	QiViewport_setCameraPos(viewport, &new_pos);
}

void QiViewport_setCameraRot_hook(QiViewport *viewport, QiQuat *rot) {
	QiQuat new_rot = QiQuat_compose(rot, &cameraRotOffset);
	viewport->cameraRot = new_rot;
	QiViewport_updateModelview(viewport);
	// QiViewport_setCameraRot(viewport, rot);
}

int knCameraPosOffset(lua_State *L) {
	if (!QiViewport_setCameraPos) {
		QiViewport_setCameraPos = YipHookFunction("_ZN10QiViewport12setCameraPosERK6QiVec3", QiViewport_setCameraPos_hook, false);
	}
	
	cameraPosOffset.x = lua_tonumber(L, 1);
	cameraPosOffset.y = lua_tonumber(L, 2);
	cameraPosOffset.z = lua_tonumber(L, 3);
	return 0;
}

static inline void quat_rotate(QiQuat *quat, float heading, float attitude, float bank) {
	float c1 = cosf(heading);
	float s1 = sinf(heading);
	float c2 = cosf(attitude);
	float s2 = sinf(attitude);
	float c3 = cosf(bank);
	float s3 = sinf(bank);
	quat->w = sqrtf(1.0 + c1 * c2 + c1*c3 - s1 * s2 * s3 + c2*c3) / 2.0;
	float w4 = (4.0 * quat->w);
	quat->x = (c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4 ;
	quat->y = (s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4 ;
	quat->z = (-s1 * s3 + c1 * s2 * c3 +s2) / w4 ;
}

int knCameraRotOffset(lua_State *L) {
	if (!QiViewport_setCameraRot) {
		QiViewport_setCameraRot = YipHookFunction("_ZN10QiViewport12setCameraRotERK6QiQuat", QiViewport_setCameraRot_hook, true);
		QiQuat_compose = YipLookupSymbol("_ZNK6QiQuatmlERKS_");
		QiViewport_updateModelview = YipLookupSymbol("_ZN10QiViewport15updateModelviewEv");
	}
	
	QiQuat offset;
	quat_rotate(&offset, lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	cameraRotOffset = offset;
	return 0;
}

float wantFov;

void (*QiViewport_setMode3D)(QiViewport *this, float fov, float near, float far);

void QiViewport_setMode3D_hook(QiViewport *this, float fov, float near, float far) {
	QiViewport_setMode3D(this, wantFov == 0.0f ? fov : wantFov, near, far);
}

int knCameraFov(lua_State *L) {
	if (!QiViewport_setMode3D) {
		QiViewport_setMode3D = YipHookFunction("_ZN10QiViewport9setMode3DEfff", QiViewport_setMode3D_hook, false);
	}
	
	wantFov = lua_tonumber(L, 1);
	
	return 0;
}

/**
 * Refresh rate changing
 */
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

/**
 * Misc utilities
 */
int knJavaCommand(lua_State *script) {
	KNLoadFunc(QiString, _Z11javaCommandRK8QiString, (QiString *command));
	
	const char *cmd = lua_tostring(script, 1);
	cmd = cmd ? cmd : "";
	QiString qCmd = MakeQiString(cmd);
	QiString result = _Z11javaCommandRK8QiString(&qCmd);
	lua_pushstring(script, result.data ? result.data : result.cached);
	return 1;
}

int knEnableGamectl(lua_State *script) {
	// Cheats
	knRegisterFunc(script, knSetBalls);
	knRegisterFunc(script, knGetBalls);
	knRegisterFunc(script, knSetStreak);
	knRegisterFunc(script, knGetStreak);
	knRegisterFunc(script, knSetNoclip);
	knRegisterFunc(script, knGetNoclip);
	
	// Reloading
	knRegisterFunc(script, knReload);
	knRegisterFunc(script, knReloadTemplates);
	knRegisterFunc(script, knReloadGfx);
	knRegisterFunc(script, knReloadPlayer);
	
	// Quick Save
	knRegisterFunc(script, knQuickSave);
	knRegisterFunc(script, knQuickLoad);
	knRegisterFunc(script, knQuickClear);
	
	// Level methods
	knRegisterFunc(script, knLevelHitSomething);
	knRegisterFunc(script, knLevelStreakAbort);
	knRegisterFunc(script, knLevelStreakInc);
	knRegisterFunc(script, knLevelAddScore);
	knRegisterFunc(script, knShoot);
	
	// Camera
	knRegisterFunc(script, knCameraPosOffset);
	knRegisterFunc(script, knCameraRotOffset);
	knRegisterFunc(script, knCameraFov);
	
	// Refresh rate
	knRegisterFunc(script, knGetDeviceHz);
	knRegisterFunc(script, knSetFrameRate);
	
	// Misc utilities
	knRegisterFunc(script, knJavaCommand);
	
	return 0;
}

#endif // GRANNY
