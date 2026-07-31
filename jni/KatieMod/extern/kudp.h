/**
 * Knot's UDP library - a non-blocking UDP library with a clean interface
 */

#ifndef _KUDP_INCLUDE_
#define _KUDP_INCLUDE_

#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef KUDP_MAX_SIZE
#define KUDP_MAX_SIZE 65507
#endif

typedef struct kudp_buffer {
	unsigned short size;
	unsigned char data[KUDP_MAX_SIZE];
} kudp_buffer;

typedef struct kudp_socket {
	int fd;
	kudp_buffer buffer;
} kudp_socket;

bool kudp_open(kudp_socket *self, const char *address, unsigned short port, bool use_connect);
kudp_buffer *kudp_recieve(kudp_socket *self);
bool kudp_send(kudp_socket *self, const void *data, size_t size);
void kudp_close(kudp_socket *self);

#endif

#ifdef KUDP_IMPLEMENTATION
#undef KUDP_IMPLEMENTATION

bool kudp_open(kudp_socket *self, const char *address, unsigned short port, bool use_connect) {
	char portstr[6];
	
	snprintf(portstr, sizeof portstr, "%hu", port);
	
	struct addrinfo hints = {
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_DGRAM,
		.ai_protocol = 0,
		.ai_flags = AI_ADDRCONFIG | AI_NUMERICHOST,
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
	
	// Set send and recieve address
	if (use_connect) {
		if (connect(self->fd, results->ai_addr, results->ai_addrlen)) {
			freeaddrinfo(results);
			close(self->fd);
			return false;
		}
	}
	// Set recieve address only
	else {
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
	
	ssize_t recvsize = recv(self->fd, self->buffer.data, KUDP_MAX_SIZE, MSG_DONTWAIT);
	
	if (recvsize == -1) {
		return NULL;
	}
	
	self->buffer.size = recvsize;
	
	return &self->buffer;
}

bool kudp_send(kudp_socket *self, const void *data, size_t size) {
	if (self->fd < 0) {
		return false;
	}
	
	ssize_t nsent = send(self->fd, data, size, MSG_DONTWAIT);
	
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
