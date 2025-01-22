#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *method;
    char *content_type;
    char *path;
    char *version;
    char *host;
    char *connection;
    char *user_agent;
} request;

request parse_request(char* buffer) {
    printf("Parsing request...\n");
}

void free_request(request req) {
    free(req.method);
    free(req.content_type);
    free(req.path);
    free(req.version);
    free(req.host);
    free(req.connection);
    free(req.user_agent);
}

int main() {
    char* buffer = "GET / HTTP/1.1\r\n"
                   "Content-Type: application/json\r\n"
                   "User-Agent: PostmanRuntime/7.42.0\r\n"
                   "Accept: */*\r\n"
                   "Postman-Token: 2e298551-587b-48ab-889d-32139cd129b3\r\n"
                   "Host: 127.0.0.1:3000\r\n"
                   "Accept-Encoding: gzip, deflate, br\r\n"
                   "Connection: keep-alive\r\n"
                   "Content-Length: 13\r\n"
                   "\r\n"
                   "hello, world!";

    request req = parse_request(buffer);
    return 0;
}