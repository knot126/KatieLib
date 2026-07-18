#include <yiploader/yiploader.h>
#include <android/log.h>

void Debug_log(void *this, char *msg, int lvl) {
	switch (lvl) {
		case 1: {
			__android_log_write(ANDROID_LOG_INFO, "smashhit", msg);
			break;
		}
		case 2: {
			__android_log_write(ANDROID_LOG_WARN, "smashhit", msg);
			break;
		}
		case 4: {
			__android_log_write(ANDROID_LOG_ERROR, "smashhit", msg);
			break;
		}
	}
}

const char *KNInitDebugLog(void) {
	YipHookFunction("_ZN5Debug3logEPKci", Debug_log, true);
	return NULL;
}
