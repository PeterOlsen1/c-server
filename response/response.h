#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HTTP "HTTP/1.1"
#define OK "200 OK"

#define REDIRECT "301 Moved Permanently"
#define TEMPORARY_REDIRECT "307 Temporary Redirect"
#define BAD_REQUEST "400 Bad Request"
#define NOT_FOUND "404 Not Found"
#define METHOD_NOT_ALLOWED "405 Method Not Allowed"
#define TOO_LARGE "413 Content Too Large"
#define URI_TOO_LONG "414 URI Too Long"
#define INTERNAL_SERVER_ERROR "500 Internal Server Error"

/**
 * Create a response to send to the client. This method is to be called one all is done.
 */
char* make_response(char* status, char* content_type, int content_length, char* body);

void send_404(int client_sock);

void send_500(int client_sock);

void send_file(int client_sock, char* path);

#endif