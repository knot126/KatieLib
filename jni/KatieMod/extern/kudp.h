/**
 * Knot's UDP library - a non-blocking UDP library with a clean interface
 */

#ifndef _KUDP_INCLUDE_
#define _KUDP_INCLUDE_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef KUDP_MAX_SIZE
#define KUDP_MAX_SIZE 65507
#endif

#ifndef KUDP_ADDR_LEN
#define KUDP_ADDR_LEN 80
#endif

typedef struct kudp_buffer {
	unsigned short size;
	unsigned char data[KUDP_MAX_SIZE];
	char address[KUDP_ADDR_LEN];
	unsigned short port;
} kudp_buffer;

typedef struct kudp_socket {
	int fd;
	kudp_buffer buffer;
} kudp_socket;

bool kudp_open(kudp_socket *self, const char *address, unsigned short port);
kudp_buffer *kudp_recieve(kudp_socket *self);
bool kudp_send(kudp_socket *self, const void *data, size_t size, const char *address, unsigned short port);
void kudp_close(kudp_socket *self);

#endif

#ifdef KUDP_IMPLEMENTATION
#undef KUDP_IMPLEMENTATION

static inline void kudp_unparseaddr(const struct sockaddr *sa, char address[KUDP_ADDR_LEN], unsigned short *port) {
	// struct sockaddr -> string, short
	
	// Convert IP address to string
	if (!inet_ntop(sa->sa_family, sa, address, KUDP_ADDR_LEN)) {
		address[0] = '\0';
	}
	
	// Get port and fix endian
	unsigned short xport = 0;
	
	switch (sa->sa_family) {
		case AF_INET: {
			xport = ((struct sockaddr_in *) sa)->sin_port;
			break;
		}
		case AF_INET6: {
			xport = ((struct sockaddr_in6 *) sa)->sin6_port;
			break;
		}
	}
	
	*port = ntohs(xport);
}

static inline bool kudp_parseaddr(struct sockaddr *sa, const char *address, const unsigned short port) {
	// string, short -> struct sockaddr
	// IPv6 can have ':' but IPv4 can't so use that to determine what it is.
	// It's a hack to be sure, though.
	memset(sa, 0, sizeof *sa);
	
	if (strchr(address, ':')) {
		sa->sa_family = AF_INET6;
		if (!inet_pton(AF_INET6, address, sa)) { return false; }
		((struct sockaddr_in6 *) sa)->sin6_port = htons(port);
	}
	else {
		sa->sa_family = AF_INET;
		if (!inet_pton(AF_INET, address, sa)) { return false; }
		((struct sockaddr_in *) sa)->sin_port = htons(port);
	}
	
	return true;
}

bool kudp_open(kudp_socket *self, const char *address, unsigned short port) {
	char portstr[6];
	
	snprintf(portstr, sizeof portstr, "%hu", port);
	
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_DGRAM,
		.ai_protocol = 0,
		.ai_flags = AI_ADDRCONFIG | AI_NUMERICHOST | AI_NUMERICSERV,
	};
	
	struct addrinfo *results = NULL;
	
	if (getaddrinfo(address, portstr, &hints, &results) || !results) {
		return false;
	}
	
	// Open socket
	self->fd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
	
	if (self->fd == -1) {
		freeaddrinfo(results);
		return false;
	}
	
	// Enable broadcast. This is really stupid IMO
	const int broadcast = 1;
	setsockopt(self->fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);
	
	// If address != NULL, this is a server and we should bind to the given
	// address
	if (address) {
		if (bind(self->fd, results->ai_addr, results->ai_addrlen)) {
			freeaddrinfo(results);
			close(self->fd);
			return false;
		}
	}
	
	freeaddrinfo(results);
	return true;
}

kudp_buffer *kudp_recieve(kudp_socket *self) {
	if (self->fd < 0) {
		return NULL;
	}
	
	struct sockaddr sa;
	socklen_t sa_size = sizeof sa;
	ssize_t nrecv = recvfrom(self->fd, self->buffer.data, KUDP_MAX_SIZE, MSG_DONTWAIT, &sa, &sa_size);
	
	if (nrecv == -1) {
		return NULL;
	}
	
	self->buffer.size = nrecv;
	kudp_unparseaddr(&sa, self->buffer.address, &self->buffer.port); // parse address
	
	return &self->buffer;
}

bool kudp_send(kudp_socket *self, const void *data, size_t size, const char *address, unsigned short port) {
	if (self->fd < 0) {
		return false;
	}
	
	struct sockaddr dest_addr;
	if (!kudp_parseaddr(&dest_addr, address, port)) {
		return false;
	}
	
	ssize_t nsent = sendto(self->fd, data, size, MSG_DONTWAIT, &dest_addr, sizeof dest_addr);
	
	return nsent == size;
}

void kudp_close(kudp_socket *self) {
	if (self->fd < 0) {
		return;
	}
	
	shutdown(self->fd, SHUT_RDWR);
	close(self->fd);
	self->fd = -1;
}

#endif
