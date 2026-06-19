#include <yiploader/yiploader.h>

#include "log.h"
#include "builtin/smashhit.h"

/* Shim-wide globals. I have nowhere else to put these. */
struct android_app *gApp;
Leaf *gLeaf;
Game **gGamePtr;
void *gLibAndroid;
void *gLibC;
const char *gGameName;
const char *gPackageCodePath;
char *gPackageName;

typedef const char *(*ModuleInitFunc)(void);

/* Sub-mod init functions */
const char *KNInitLua(void);
const char *KNDatabaseInit(void);
const char *KNAntitamperInit(void);
const char *KNOverlayInit(void);
const char *KNInitShutdown(void);

ModuleInitFunc submod_init_functions[] = {
	KNInitLua,
	KNDatabaseInit,
	KNAntitamperInit,
	KNOverlayInit,
	KNInitShutdown,
	NULL,
};

const char *kaite_init_globals(void) {
	/**
	 * Initialise some core stuff the mod needs.
	 */
	
	gApp = YipGetAndroidAppStruct();
	gLeaf = YipGetLeafInstance();
	gGameName = YipGetGameName();
	gPackageName = KNGetPackageName();
	
	gLibAndroid = dlopen("libandroid.so", RTLD_NOW | RTLD_GLOBAL);
	
	if (!gLibAndroid) {
		return "Loading libandroid.so failed";
	}
	
	gLibC = dlopen("libc.so", RTLD_NOW | RTLD_GLOBAL);
	
	if (!gLibC) {
		return "Loading libc.so failed";
	}
	
	gGamePtr = YipLookupSymbol("gGame");
	
	if (!gGamePtr) {
		return "Failed to find address of gGame";
	}
	
	return NULL;
}


const char *mod_init(void) {
	const char *err = NULL;
	
	if ((err = kaite_init_globals())) {
		return err;
	}
	
	// Init sub-mods
	for (size_t i = 0; submod_init_functions[i] != NULL; i++) {
		const char *status = (submod_init_functions[i])();
		
		if (status) {
			LogF("KnShim module at index %zu failed to load: %s", i, status);
			abort();
		}
	}
	
	return NULL;
}
