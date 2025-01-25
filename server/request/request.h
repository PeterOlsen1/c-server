#ifndef REQUEST_H
#define REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../response/response.h"

#define BODY_MAX_SIZE 4096

/**
 * Define a type to represent a request object
 */
typedef struct {
    char *method;
    char *content_type;
    char *path;
    char *version;
    char *host;
    char *connection;
    char *user_agent;
    unsigned int content_length;
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