/**
 * Camera control
 */

#include <math.h>
#include "../common/lua_utils.h"
#include "smashhit.h"
#include "../util.h"

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

static inline void quat_rotate(QiQuat *quat, float heading, float bank, float attitude) {
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

int knEnableCamera(lua_State *script) {
	knRegisterFunc(script, knCameraPosOffset);
	knRegisterFunc(script, knCameraRotOffset);
	knRegisterFunc(script, knCameraFov);
	
	return 0;
}
