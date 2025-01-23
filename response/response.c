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
 * Send an arbitrary file to the client.
 * 
 * If the given file does not exist, send a 404 response.
 */
void send_file(int client_sock, char* path) {
    char file_path[128];

    if (strcmp(path, "/") == 0) {
        path = "/index.html";
    } 
    
    snprintf(file_path, 128, "./files%s", path);
    FILE* file = fopen(file_path, "r");

    //open file
    if (!file) {
        send_404(client_sock);
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
