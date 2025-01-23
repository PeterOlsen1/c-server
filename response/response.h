#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BINARY_FILES ".jpg .jpeg .png .gif .ico"

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

/**
 * Generate a resopnse header to send to the client.
 * 
 * This is only used for binary files, since we need to send
 * text data and binary data in different socket calls.
 */
char* make_response_header(char* status, char* content_type, int content_length);

/**
 * Return the MIME type of a given file path.
 * 
 * It does not need to exist, it just has to have a .(something)
 * extension.
 * 
 * If no proper path is found, just return as plain text.
 */
char* get_mime_type(char* path);

/**
 * Send a 404 response to the client.
 */
void send_404(int client_sock);

/**
 * Send a 500 response to the client.
 */
void send_500(int client_sock);

/**
 * Sends an arbitrary error response
 */
void send_error(int client_sock, char* status, char* body);

/**
 * Send a binary file to the client
 */
void send_binary(int client_sock, char* path);

/**
 * Send an arbitrary file to the client. (NOT BINARY YET)
 */
void send_file(int client_sock, char* path);

#endif