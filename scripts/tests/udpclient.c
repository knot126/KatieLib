#include <stdio.h>
#include <errno.h>

#define KUDP_IMPLEMENTATION
#include "../../jni/KatieMod/extern/kudp.h"

int main(int argc, const char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <host>\n", argv[0]);
		return 127;
	}
	
	kudp_socket sock;
	if (!kudp_open(&sock, argv[1], 8000)) {
		fprintf(stderr, "Socket open failed!!\n");
		return 2;
	}
	
	while (true) {
		kudp_buffer *buf = kudp_recieve(&sock);
		if (buf) {
			buf->data[KUDP_MAX_SIZE-1] = '\0';
			printf("Got packet from %s:%hu: (%d bytes) %s\n", buf->address, buf->port, buf->size, buf->data);
		}
	}
	
	return 0;
}
