#include "lua_utils.h"
#include "../util.h"

#define KUDP_IMPLEMENTATION
#include "../extern/kudp.h"

int knUdpSocketIndex(lua_State *L);
int knUdpSocketClose(lua_State *L);

int KnUdpSocket(lua_State *L) {
	/**
	 * udpSocket = KnUdpSocket([address: string, port: integer])
	 * 
	 * Creates a new udp socket, optionally bound to an address and port from
	 * where it can recieve messages.
	 */
	
	const char *address = lua_tostring(L, 1);
	unsigned short port = lua_tointeger(L, 2);
	
	kudp_socket *sckt = lua_newuserdata(L, sizeof *sckt);
	
	if (!kudp_open(sckt, address, port)) {
		lua_pop(L, 1);
		return luaL_error(L, "Failed to open UDP socket with address %s and port %hu", address ? address : "<none>", port);
	}
	
	if (luaL_newmetatable(L, "kudp_socket")) {
		lua_pushcfunction(L, knUdpSocketIndex);
		lua_setfield(L, -2, "__index");
		lua_pushcfunction(L, knUdpSocketClose);
		lua_setfield(L, -2, "__gc");
	}
	
	// okay i did a bug and this was -1 so the metatable was getting set to
	// its own metatable
	// pro player de metatable
	lua_setmetatable(L, -2);
	
	return 1;
}

int knUdpSocketSend(lua_State *L) {
	/**
	 * success = udpSocket:send(data: string, address: string, port: integer): boolean
	 * 
	 * Send a datagram to an address
	 */
	
	kudp_socket *sckt = lua_touserdata(L, 1);
	size_t size;
	const char *data = lua_tolstring(L, 2, &size);
	const char *address = lua_tostring(L, 3);
	unsigned short port = lua_tointeger(L, 4);
	
	if (!address) {
		return luaL_error(L, "Invalid address or address not specified");
	}
	
	lua_pushboolean(L, kudp_send(sckt, data, size, address, port));
	
	return 1;
}

int knUdpSocketRecieve(lua_State *L) {
	/**
	 * data: string, address: string, port: integer = udpSocket:recieve()
	 * 
	 * Recieve the next datagram in the queue. Returns nil when none are
	 * available or there is an error. Socket must be bound to an address.
	 */
	
	kudp_socket *sckt = lua_touserdata(L, 1);
	kudp_buffer *buf = kudp_recieve(sckt);
	
	if (buf) {
		lua_pushlstring(L, (char *) buf->data, buf->size);
		lua_pushstring(L, buf->address);
		lua_pushinteger(L, buf->port);
	}
	else {
		lua_pushnil(L);
		lua_pushnil(L);
		lua_pushnil(L);
	}
	
	return 3;
}

int knUdpSocketClose(lua_State *L) {
	/**
	 * udpSocket:close()
	 * 
	 * Close the UDP socket
	 */
	
	kudp_socket *sckt = lua_touserdata(L, 1);
	kudp_close(sckt);
	return 0;
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
