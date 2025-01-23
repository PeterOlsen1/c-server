#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "response.h"

/**
 * Create a response to send to the client. This method is to be called one all is done.
 */
char* make_response(char* status, char* content_type, int content_length, char* body) {
    int response_length = strlen(HTTP) + strlen(status) + strlen(content_type) + strlen(body) + 80;
    char* response = malloc(response_length);
    sprintf(response, "%s %s\nContent-Type: %s\nContent-Length: %d\n\n%s", HTTP, status, content_type, content_length, body);

    return response;
}

/**
 * Create a response header to send to the client.
 */
char* make_response_header(char* status, char* content_type, int content_length) {
    int header_length = strlen(HTTP) + strlen(status) + strlen(content_type) + 50;
    char* header = malloc(header_length);
    sprintf(header, "%s %s\nContent-Type: %s\nContent-Length: %d\n\n", HTTP, status, content_type, content_length);
    return header;
}

/**
 * Return the MIME type of a given file path.
 */
char* get_mime_type(char* path) {
    char* ext = strrchr(path, '.');

    if (!ext) {
        return "text/plain";
    }

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
        return "text/html";
    } else if (strcmp(ext, ".css") == 0) {
        return "text/css";
    } else if (strcmp(ext, ".js") == 0) {
        return "application/javascript";
    } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        return "image/jpeg";
    } else if (strcmp(ext, ".png") == 0) {
        return "image/png";
    } else if (strcmp(ext, ".gif") == 0) {
        return "image/gif";
    } else if (strcmp(ext, ".ico") == 0) {
        return "image/x-icon";
    } else {
        return "text/plain";
    }
}


/**
 * Send an arbitrary file to the client.
 * 
 * If the given file does not exist, send a 404 response.
 */
void send_file(int client_sock, char* path) {
    char file_path[128];

    if (strcmp(path, "/") == 0) {
        path = "/index.html";
    } 

    //format the file path
    snprintf(file_path, 128, "./files%s", path);
    FILE* file = fopen(file_path, "r");

    //check if binary file
    if (strstr((BINARY_FILES), strchr(path, '.'))) {
        printf("Sending binary file: %s\n", file_path);
        send_binary(client_sock, file_path);
        return;
    }

    //open file
    if (!file) {
        send_404(client_sock);
        return;
    }

    //get file size
    fseek(file, 0, SEEK_END);   //set ptr to end of file
    int fsize = ftell(file);    //get ptr position
    fseek(file, 0, SEEK_SET);   //return to beginning (cheeky!)

    //read file into response body
    char* body = malloc(fsize + 1);
    fread(body, 1, fsize, file);
    body[fsize] = '\0';

    //get mime type
    char* mime = get_mime_type(path);

    //make response
    char* resp = make_response(OK, mime, strlen(body), body);

    //send it!!!
    send(client_sock, resp, strlen(resp), 0);

    //housekeeping
    free(body);
    free(resp);
    fclose(file);
    close(client_sock);
    return;
}


/**
 * Send a binary file to the client
 */
void send_binary(int client_sock, char* path) {
    FILE* file = fopen(path, "rb");

    if (!file) {
        send_404(client_sock);
        return;
    }

    fseek(file, 0, SEEK_END);   //set ptr to end of file
    int fsize = ftell(file);    //get ptr position
    fseek(file, 0, SEEK_SET);   //return to beginning (cheeky!)

    //read file into response body
    char* body = malloc(fsize);
    fread(body, 1, fsize, file);

    //get mime type
    char* mime = get_mime_type(path);

    //SAME AS ABOVE UNTIL HERE. SEND HEADER AND BODY SEPARATELY

    //make header separately from the body (for binary files)
    char* header = make_response_header(OK, mime, fsize);

    //send header
    send(client_sock, header, strlen(header), 0);

    //send body
    send(client_sock, body, fsize, 0);

    //housekeeping
    free(body);
    fclose(file);
    close(client_sock);
    return;
}

/**
 * Sends an arbitrary error response to the client
 */
void send_error(int client_sock, char* status, char* body) {
    FILE* file = fopen("./files/error.html", "r");

    if (!file) {
        printf("Failed to open file\n");
        send_500(client_sock);
        return;
    }

    //get file size
    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    //read file into response body
    char* file_content = malloc(fsize + 1);
    fread(file_content, 1, fsize, file);
    file_content[fsize + 1] = '\0';

    //format the response body
    size_t resp_body_size = fsize + strlen(body) - 1;
    char* resp_body = malloc(resp_body_size);
    snprintf(resp_body, resp_body_size, resp_body, body);

    //make response
    char* resp = make_response(NOT_FOUND, "text/html", resp_body_size, resp_body);

    //send it!!!
    send(client_sock, resp, strlen(resp), 0);

    //housekeeping
    free(resp_body);
    free(resp);
    fclose(file);
    close(client_sock);
    return;
}

/**
 * Send a 404 response to the client
 */
void send_404(int client_sock) {
    FILE* file = fopen("./files/404.html", "r");

    if (!file) {
        printf("Failed to open file\n");
        send_500(client_sock);
        return;
    }

    //get file size
    fseek(file, 0, SEEK_END);
    int fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    //read file into response body
    char* body = malloc(fsize + 1);
    fread(body, 1, fsize, file);
    body[fsize] = '\0';

    //make response
    char* resp = make_response(NOT_FOUND, "text/html", fsize + 1, body);

    //send it!!!
    send(client_sock, resp, strlen(resp), 0);

    //housekeeping
    free(body);
    free(resp);
    fclose(file);
    close(client_sock);
    return;
}


/**
 * Send a 500 response to the client
 */
void send_500(int client_sock) {
    char* body = "<h1>500 Internal Server Error</h1>";
    char* resp = make_response(INTERNAL_SERVER_ERROR, "text/html", strlen(body), body);
    send(client_sock, resp, strlen(resp), 0);
    close(client_sock);
    return;
}
