// clang -o http.so ./https_test.c && valgrind --tool=memcheck --leak-check=full ./http.so http://google.com/

#define HTTP_IMPLEMENTATION
#include "http.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Not enough arguments!\n");
		exit(69);
	}
	
	fprintf(stderr, "GET %s\n", argv[1]);
	
	http_header_t headers[] = {
		(http_header_t) {"From", "johngoogle-sorry-gravis-i-just-need-to-test-my-http-library@google.com"}
	};
	
	http_t *http = http_request("GET", argv[1], NULL, 0, headers, 1, NULL, NULL);
	
	if (!http) {
		fprintf(stderr, "Failed to create http request\n");
		exit(1);
	}
	
	http_status_t status;
	
	while ((status = http_process(http)) == HTTP_STATUS_PENDING) {
		fprintf(stderr, "Recieved %zu bytes so far\n", http->response_size);
	}
	
	if (status == HTTP_STATUS_FAILED) {
		fprintf(stderr, "HTTP Request Failed!\n");
		exit(2);
	}
	else {
		printf("HTTP/1.0 %d %s\n", http->status_code, http->reason_phrase);
		for (size_t i = 0; i < http->num_headers; i++) {
			printf("%s: %s\n", http->headers[i].name, http->headers[i].value);
		}
		fwrite(http->response_data, http->response_size, 1, stdout);
	}
	
	http_release(http);
	
	return 0;
}
