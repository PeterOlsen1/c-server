#include "request.h"


/**
 * Parse an HTTP request string and return a request object
 */
request* parse_request(char* buffer, int client_sock) {
    request* req = malloc(sizeof(request));
    if (!req) {
        printf("Failed to allocate memory for request object\n");
        response* res = init_response(client_sock);
        send_error(res, INTERNAL_SERVER_ERROR, "Failed to allocate memory for request object");
        free(res);
        return NULL;
    }

    printf("Allocating request object\n");

    req->method = malloc(10);
    req->path = malloc(100);
    req->version = malloc(10);
    req->socket = client_sock;
    req->body = malloc(BODY_MAX_SIZE);

    char* buffer_copy = strdup(buffer);
    if (!buffer_copy) {
        printf("Failed to allocate memory for buffer copy\n");
        free(req);
        response* res = init_response(client_sock);
        send_error(res, INTERNAL_SERVER_ERROR, "Failed to allocate memory for buffer copy");
        free(res);
        return NULL;
    }

    //leave delim as \n and not \n\r so that we can keep line before body
    const char* delim = "\n";
    char* line = strtok(buffer_copy, delim);

    if (!line) {
        printf("Failed to parse request\n");
        free(req);
        free(buffer_copy);
        response* res = init_response(client_sock);
        send_error(res, BAD_REQUEST, "Failed to parse request");
        free(res);
        return NULL;
    }
    
    //scan first line real quick
    sscanf(line, "%s %s %s", req->method, req->path, req->version);

    JSON* headers = init_json();
    req->headers = headers;

    //extract more info from the request
    while ((line = strtok(NULL, delim)) != NULL) {
        //next line is body
        if (strcmp(line, "\r") == 0) {
            break;
        }

        //remove the \r at the end of the line
        line[strlen(line) - 1] = '\0';

        //EXTRACT INFO DYNAMICALLY AND PLACE IN
        //req->headers, keep key and value

        //suddenly i care about memory efficiency
        char* header = malloc(MAX_HEADER_LENGTH);
        char* value = malloc(MAX_HEADER_LENGTH);
        if (!header || !value) {
            printf("Failed to allocate memory for header or value\n");
            free(req);
            free(buffer_copy);
            response* res = init_response(client_sock);
            send_error(res, INTERNAL_SERVER_ERROR, "Failed to allocate memory for header or value");
            free(res);
            return NULL;
        }

        printf("About to reallocate memory\n");
        printf("%s\n", line);
        //scan header and value
        sscanf(line, "%s %s", header, value);
        header = realloc(header, strlen(header) + 1);
        value = realloc(value, strlen(value) + 1);

        if (!header || !value) {
            printf("Failed to reallocate memory for header or value\n");
            free(req);
            free(buffer_copy);
            response* res = init_response(client_sock);
            send_error(res, INTERNAL_SERVER_ERROR, "Failed to reallocate memory for header or value");
            free(res);
            return NULL;
        }

        printf("adding null terminator\n");
        header[strlen(header) - 1] = '\0'; //remove extra :

        printf("inserting into json\n");
        //insert header into headers object
        insert(headers, STRING, header, value); 
        printf("inserted into json\n");
    }   

    // if (req->content_length > BODY_MAX_SIZE) {
    //     printf("Content length exceeds maximum size\n");
    //     free(req);
    //     free(buffer_copy);
    //     response* res = init_response(client_sock);
    //     send_error(res, TOO_LARGE, "Content length exceeds maximum size");
    //     free(res);
    //     return NULL;
    // }

    //add 4 to body to get past the \r\n\r\n
    char* body = strstr(buffer, "\r\n\r\n") + 4;
    if (!body) {    //idk where this would even happen
        printf("Failed to find request body\n");
        free(req);
        free(buffer_copy);
        response* res = init_response(client_sock);
        send_error(res, BAD_REQUEST, "Failed to find body");
        free(res);
        return NULL;
    }

    //BODY!!!
    req->body = strdup(body);

    free(buffer_copy);
    return req;
}

/**
 * Log the request to the console
 */
void log_request(request* req) {
    if (!req) {
        printf("Invalid request recieved\n");
    }
    else {
        printf("Incoming request: %s %s\n", req->method, req->path);

        //only log if it is enabled
        if (LOGGING_ENABLED) {
            FILE* log = fopen("log.txt", "a");
            if (!log) {
                perror("Failed to open log file");
                return;
            }
    
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            fprintf(log, "%d-%02d-%02d %02d:%02d:%02d, ", tm.tm_year + 1900, tm.tm_mon + 1, 
                tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            fprintf(log, "%s %s\n", req->method, req->path);
    
            fclose(log);
        }
    }
}


/**
 * Frees the request object
 */
void free_request(request* req) {
    if (req->method) {
        free(req->method);
    }
    if (req->path) {
        free(req->path);
    }
    if (req->version) {
        free(req->version);
    }
    if (req->headers) {
        free_json(req->headers);
    }
    free(req);
}

/**
 * Print a request object
 */
void print_request(request* req) {
    printf("Method: %s\n", req->method);
    printf("Path: %s\n", req->path);
    printf("Version: %s\n", req->version);
    printf("Body: %s\n", req->body);
    printf("Headers: %s\n", stringify(req->headers));
    return;
}