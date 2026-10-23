/**
 * Knot's UDP library - a non-blocking UDP library with a clean interface
 */

#ifndef _KUDP_INCLUDE_
#define _KUDP_INCLUDE_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <poll.h>

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

/**
 * Open a new socket and bind to a port and address and port if address is not
 * null.
 */
bool kudp_open(kudp_socket *self, const char *address, unsigned short port);

/**
 * Recieve a datagram.
 */
kudp_buffer *kudp_recieve(kudp_socket *self);

/**
 * Send a datagram to the given address and port.
 */
bool kudp_send(kudp_socket *self, const void *data, size_t size, const char *address, unsigned short port);

/**
 * Return the time that the last recieved packet was passed to the user.
 */
double kudp_timestamp(kudp_socket *self);

/**
 * Close the UDP socket.
 */
void kudp_close(kudp_socket *self);

#endif

#ifdef KUDP_IMPLEMENTATION
#undef KUDP_IMPLEMENTATION

static inline void kudp_unparseaddr(const struct sockaddr *sa, char address[KUDP_ADDR_LEN], unsigned short *port) {
	// struct sockaddr -> string, short
	
	// Convert IP address to string
	void *ba = sa->sa_family == AF_INET6 ? (void*)&((struct sockaddr_in6 *) sa)->sin6_addr : (void*)&((struct sockaddr_in *) sa)->sin_addr;
	
	if (!inet_ntop(sa->sa_family, ba, address, KUDP_ADDR_LEN)) {
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
		if (!inet_pton(AF_INET6, address, &((struct sockaddr_in6 *) sa)->sin6_addr)) { return false; }
		((struct sockaddr_in6 *) sa)->sin6_port = htons(port);
	}
	else {
		sa->sa_family = AF_INET;
		if (!inet_pton(AF_INET, address, &((struct sockaddr_in *) sa)->sin_addr)) { return false; }
		((struct sockaddr_in *) sa)->sin_port = htons(port);
	}
	
	return true;
}

bool kudp_open(kudp_socket *self, const char *address, unsigned short port) {
	char portstr[6];
	
	snprintf(portstr, sizeof portstr, "%hu", port);
	
	struct sockaddr sa;
	
	if (!kudp_parseaddr(&sa, address, port)) {
		return false;
	}
	
	// Open socket
	self->fd = socket(sa.sa_family, SOCK_DGRAM, 0);
	
	if (self->fd == -1) {
		return false;
	}
	
	// Enable broadcast, for those who want it
	const int one = 1;
	setsockopt(self->fd, SOL_SOCKET, SO_BROADCAST, &one, sizeof one);
	setsockopt(self->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
	
	// If address != NULL, this is a server and we should bind to the given
	// address
	if (address) {
		if (bind(self->fd, &sa, sizeof sa)) {
			close(self->fd);
			return false;
		}
	}
	
	return true;
}

kudp_buffer *kudp_recieve(kudp_socket *self) {
	if (self->fd < 0) {
		// LogW("self->fd < 0");
		return NULL;
	}
	
	struct sockaddr sa;
	socklen_t sa_size = sizeof sa;
	// LogW("recvfrom(%d, self->buffer.data, %d, MSG_DONTWAIT, &sa, &sa_size)", self->fd, KUDP_MAX_SIZE);
	ssize_t nrecv = recvfrom(self->fd, self->buffer.data, KUDP_MAX_SIZE, MSG_DONTWAIT, &sa, &sa_size);
	
	if (nrecv == -1) {
		// if (errno != EAGAIN) LogW("nrecv == -1, errno=%s", strerror(errno));
		return NULL;
	}
	
	self->buffer.size = nrecv;
	kudp_unparseaddr(&sa, self->buffer.address, &self->buffer.port); // convert address to output format
	
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

double kudp_timestamp(kudp_socket *self) {
	if (self->fd < 0) {
		return 0.0;
	}
	
	struct timeval result;
	
	if (ioctl(self->fd, SIOCGSTAMP, &result)) {
		return 0.0;
	}
	
	return (double)result.tv_sec + ((double) result.tv_usec) / 1000000000.0;
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
