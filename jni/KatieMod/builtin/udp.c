#include "lua_utils.h"
#include "../util.h"

#define KUDP_IMPLEMENTATION
#include "../extern/kudp.h"

int KnUdpSocket(lua_State *L) {
	const char *address = lua_tostring(L, 1);
	unsigned short port = lua_tointeger(L, 2);
	bool use_bind = lua_tostring(L, 3);
	kudp_socket *sckt = lua_newuserdata(L, sizeof *sckt);
	
	if (!kudp_open(sckt, address, port, !use_bind)) {
		lua_pop(L, 1);
		return luaL_error(L, "Failed to open UDP socket with address %s and port %hu", address, port);
	}
	
	if (lua_newmetatable(L, "kudp_socket")) {
		lua_pushcfunction(L, knUdpSocketIndex);
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, knUdpSocketClose);
		lua_setfield(L, -2, "__gc");
	}
	
	lua_setmetatable(L, -1);
	
	return 1;
}

int knUdpSocketSend(lua_State *L) {
	kudp_socket *sckt = lua_touserdata(L, 1);
	size_t size;
	const char *data = lua_tolstring(L, 2, &size);
	lua_pushboolean(L, kudp_send(sckt, data, size));
	return 1;
}

int knUdpSocketRecieve(lua_State *L) {
	kudp_socket *sckt = lua_touserdata(L, 1);
	kudp_buffer *buf = kudp_recieve(sckt);
	if (buf) {
		lua_pushlstring(L, buf->data, buf->size);
	}
	else {
		lua_pushnil(L);
	}
	return 1;
}

int knUdpSocketClose(lua_State *L) {
	kudp_socket *sckt = lua_touserdata(L, 1);
	kudp_close(sckt);
}

int knUdpSocketIndex(lua_State *L) {
	const char *key = lua_tostring(L, 2);
	
	if (!strcmp(key, "send")) {
		lua_pushcfunction(L, knUdpSocketSend);
		return 1;
	}
	
	if (!strcmp(key, "recieve")) {
		lua_pushcfunction(L, knUdpSocketRecieve);
		return 1;
	}
	
	if (!strcmp(key, "close")) {
		lua_pushcfunction(L, knUdpSocketClose);
		return 1;
	}
	
	if (!strcmp(key, "fd")) {
		kudp_socket *sckt = lua_touserdata(L, 1);
		lua_pushinteger(L, sckt->fd);
		return 1;
	}
	
	lua_pushnil(L);
	return 1;
}

int knEnableUdp(lua_State *L) {
	knRegisterFunc(L, KnUdpSocket);
	return 0;
}
