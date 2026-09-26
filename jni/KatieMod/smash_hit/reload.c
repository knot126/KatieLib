/**
 * Reload various aspects of the game that aren't normally reloadable
 */

#include "../common/lua_utils.h"
#include "smashhit.h"
#include "../util.h"

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

int knEnableReload(lua_State *script) {
	knRegisterFunc(script, knReload);
	knRegisterFunc(script, knReloadTemplates);
	knRegisterFunc(script, knReloadGfx);
	knRegisterFunc(script, knReloadPlayer);
	
	return 0;
}
