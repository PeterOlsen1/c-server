#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdarg.h>

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
 * Basic response object to abstract the client socket from the user.
 * 
 * Send this with every response method so that the user doesn't need
 * to worry about the socket, just the 'res' variable, (which is a lot prettier)
 */
typedef struct {
    int client_sock;
} response;

/**
 * Frees a repsposne object
 */
void free_response(response* res);

/**
 * Initialize a response object.
 */
response* init_response(int client_sock);

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

// SEND TEXT AND FILES

/**
 * Send a plaintext response to the client.
 */
void send_text(response* res, char* text);


/**
 * Send a json response to the clinet.
 */
void send_json(response* res, char* json);

/**
 * Send a template file to the client.
 * 
 * The number of extra arguments should correspond
 * to the number of placeholders in the template file
 */
void send_template(response* res, char* path, ...);

/**
 * Send an arbitrary file to the client. (NOT BINARY YET)
 */
void send_file(response* res, char* path);

/**
 * Send a binary file to the client
 */
void send_binary(response* res, char* path);


// SEND ERRORS TO THE CLIENT 

/**
 * Send a 404 response to the client.
 */
void send_404(response* res);

/**
 * Send a 500 response to the client.
 */
void send_500(response* res);

/**
 * Sends an arbitrary error response.
 * 
 * Status is the status code (found above) which will 
 * be sent to the client, and body is the text that will be 
 * displayed on the web page.
 */
void send_error(response* res, char* status, char* body);


#endif // RESPONSE_H