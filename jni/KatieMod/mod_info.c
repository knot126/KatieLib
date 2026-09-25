// Automatically generated information about this modification.
#include <yiploader/yiploader.h>
#include "version.h"

YipModInfo yiploader_version = {
	.name = "YipLoader",
	.version = 1,
};

YipModInfo mod_info = {
	.name = "KatieLib",
	.author = "knot126",
	.description = "Support for KnShim features when using YipLoader with supported games",
	.game = "smashhit",
	.version = SHIM_VERSION_INT,
	.assumes = &yiploader_version,
	.conflicts = NULL,
};
