#ifndef REQUEST_H
#define REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../response/response.h"
#include "../json/json.h"

#define BODY_MAX_SIZE 4096
#define LOGGING_ENABLED 0
#define MAX_HEADER_LENGTH 1024

#define GET "GET"
#define POST "POST"
#define PUT "PUT"
#define DELETE "DELETE"
#define OPTIONS "OPTIONS"
#define HEAD "HEAD"
#define TRACE "TRACE"
#define CONNECT "CONNECT"
#define PATCH "PATCH"

/**
 * Define a type to represent a request object
 */
typedef struct {
    char *method;
    char *path;
    char *version;
    JSON* headers;
    int socket;
    char *body;
} request;

/**
 * Parse an HTTP request string and return a request object
 */
request* parse_request(char* buffer, int client_sock);

/**
 * Frees the request object
 */
void free_request(request* req);

/**
 * Prints the request object
 */
void print_request(request* req);

/**
 * Logs the request to the console
 */
void log_request(request* req);

#endif // REQUEST_H