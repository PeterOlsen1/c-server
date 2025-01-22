#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "request.h"


#define HTTP_METHODS ["GET", "POST", "PUT", "DELETE", "PATCH", "OPTIONS", "HEAD", "TRACE", "CONNECT"]


/**
 * Parse an HTTP request string and return a request object
 */
request* parse_request(char* buffer) {
    request* req = malloc(sizeof(request));
    if (!req) {
        printf("Failed to allocate memory for request object\n");
        return NULL;
    }

    req->method = malloc(10);
    req->content_type = malloc(50);
    req->path = malloc(100);
    req->version = malloc(10);
    req->host = malloc(50);
    req->connection = malloc(50);
    req->user_agent = malloc(50);
    req->body = malloc(BODY_MAX_SIZE);

    char* buffer_copy = strdup(buffer);
    if (!buffer_copy) {
        printf("Failed to allocate memory for buffer copy\n");
        free(req);
        return NULL;
    }

    const char* delim = "\r\n";
    char* line = strtok(buffer_copy, delim);

    if (!line) {
        printf("Failed to parse request\n");
        free(req);
        free(buffer_copy);
        return NULL;
    }
    
    //scan first line real quick
    sscanf(line, "%s %s %s", req->method, req->path, req->version);

    //extract more info from the request
    while (line) {
        line = strtok(NULL, delim);
        if (!line) {
            break;
        }

        //find substrings and extract info accordingly
        if (strstr(line, "Content-Type:")) {
            sscanf(line, "Content-Type: %s", req->content_type);
        } else if (strstr(line, "Host:")) {
            sscanf(line, "Host: %s", req->host);
        } else if (strstr(line, "Connection:")) {
            sscanf(line, "Connection: %s", req->connection);
        } else if (strstr(line, "User-Agent:")) {
            sscanf(line, "User-Agent: %s", req->user_agent);
        } else if (strstr(line, "Content-Length:")) {
            sscanf(line, "Content-Length: %u", &req->content_length);
        }
    }

    //add 4 to body to get past the \r\n\r\n
    char* body = strstr(buffer, "\r\n\r\n") + 4;
    if (!body) {    //idk where this would even happen
        printf("Failed to find body\n");
        free(req);
        free(buffer_copy);
        return NULL;
    }

    //BODY!!!
    req->body = strdup(body);


    print_request(req);
    free(buffer_copy);
    return req;
}



/**
 * Frees the request object
 */
void free_request(request* req) {
    free(req->method);
    free(req->content_type);
    free(req->path);
    free(req->version);
    free(req->host);
    free(req->connection);
    free(req->user_agent);
    free(req->body);
    free(req);
}

/**
 * Print a request object
 */
void print_request(request* req) {
    printf("Method: %s\n", req->method);
    printf("Path: %s\n", req->path);
    printf("Version: %s\n", req->version);
    printf("Content-Type: %s\n", req->content_type);
    printf("Host: %s\n", req->host);
    printf("Connection: %s\n", req->connection);
    printf("User-Agent: %s\n", req->user_agent);
    printf("Content-Length: %d\n", req->content_length);
    printf("Body: %s\n", req->body);
    return;
}


/**
 * For testing purposes
 */
int main() {
    char* buffer = "GET / HTTP/1.1\r\nContent-Type: application/json\r\nUser-Agent: PostmanRuntime/7.42.0\r\nAccept: */*\r\nPostman-Token: 2e298551-587b-48ab-889d-32139cd129b3\r\nHost: 127.0.0.1:3000\r\nAccept-Encoding: gzip, deflate, br\r\nConnection: keep-alive\r\nContent-Length: 13\r\n\r\n{\"hello\": \"world\"}";

    request* req = parse_request(buffer);
    free_request(req);
    return 0;
}