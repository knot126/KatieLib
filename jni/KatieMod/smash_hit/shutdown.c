/**
 * Provides calling shutdown() on QiScript exit, which is not normally a feature
 * for some reason.
 */

#include <yiploader/yiploader.h>
#include "smashhit.h"
#include "../util.h"

void (*QiScript_shutdown)(QiScript *this);
bool (*QiScript_execute)(QiScript *this, QiString *code);

void QiScript_shutdown_hook(QiScript *this) {
	QiString qCode = {.data = "shutdown()", .length = 10};
	QiScript_execute(this, &qCode);
	QiScript_shutdown(this);
}

const char *KNInitShutdown(void) {
	QiScript_execute = YipLookupSymbol("_ZN8QiScript7executeERK8QiString");
	
	if (!QiScript_execute) { return "Cannot find QiScript::execute(QiString)!!"; }
	
	QiScript_shutdown = YipHookFunction("_ZN8QiScript8shutdownEv", &QiScript_shutdown_hook, false);
	
	if (!QiScript_shutdown) { return "Cannot hook QiScript::shutdown()!!"; }
	
	return NULL;
}
