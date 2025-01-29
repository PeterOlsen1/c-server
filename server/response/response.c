#include "response.h"

/**
 * Frees a repsposne object
 */
void free_response(response* res) {
    free(res);
    return;
}

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


void send_text(response* res, char* text) {
    int client_sock = res->client_sock;

    char* resp = make_response(OK, "text/plain", strlen(text), text);
    send(client_sock, resp, strlen(resp), 0);
    close(client_sock);
    return;
}

void send_json(response* res, char* json) {
    int client_sock = res->client_sock;

    char* resp = make_response(OK, "application/json", strlen(json), json);
    send(client_sock, resp, strlen(resp), 0);
    close(client_sock);
    return;
}


/**
 * Send an arbitrary file to the client.
 * 
 * If the given file does not exist, send a 404 response.
 */
void send_file(response* res, char* path) {
    int client_sock = res->client_sock;
    
    if (path == NULL) {
        send_404(client_sock);
        return;
    }

    int path_len = strlen(path) + 8;
    if (strcmp(path, "/") == 0) {
        path = "/index.html";
        path_len = strlen(path) + 8;
    } 

    char* file_path = malloc(path_len);

    //format the file path
    snprintf(file_path, path_len, "./files%s", path);

    //grab path extension
    char* ext = strrchr(path, '.');

    if (!ext) {
        send_404(client_sock);
        free(file_path);
        return;
    }

    //check if file is binary
    if (strstr((BINARY_FILES), ext)) {
        send_binary(client_sock, file_path);
        free(file_path);
        return;
    }

    //open file
    FILE* file = fopen(file_path, "r");
    if (!file) {
        printf("Failed to open file\n");
        send_404(client_sock);
        free(file_path);
        return;
    }

    //get file size
    fseek(file, 0, SEEK_END);   //set ptr to end of file
    int fsize = ftell(file);    //get ptr position
    fseek(file, 0, SEEK_SET);   //return to beginning (cheeky!)

    //read file into response body
    char* body = malloc(fsize + 1);
    if (!body) {
        printf("Failed to allocate memory for resposne body\n");
        send_500(client_sock);
        return;
    }
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
    free(file_path);
    fclose(file);
    close(client_sock);
    return;
}


/**
 * Send a binary file to the client.
 * 
 * This one operates differnetly in the way that we open
 * the file and the way that we send data back to the client.
 * 
 * Since we can't send binary data and text data at the same time,
 * we send them with separate socket calls.
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
void send_error(response* res, char* status, char* body) {
    int client_sock = res->client_sock;

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
    snprintf(resp_body, resp_body_size, file_content, body);

    //make response
    char* resp = make_response(status, "text/html", resp_body_size, resp_body);

    //send it!!!
    send(client_sock, resp, strlen(resp), 0);

    //housekeeping
    free(resp_body);
    free(file_content);
    free(resp);
    fclose(file);
    close(client_sock);
    return;
}


/**
 * Send a 404 response to the client
 */
void send_404(response* res) {
    int client_sock = res->client_sock;
    send_error(client_sock, NOT_FOUND, "<h1>404 Not Found</h1>");
    return;
}


/**
 * Send a 500 response to the client
 */
void send_500(response* res) {
    int client_sock = res->client_sock;
    char* body = "<h1>500 Internal Server Error</h1>";
    char* resp = make_response(INTERNAL_SERVER_ERROR, "text/html", strlen(body), body);
    send(client_sock, resp, strlen(resp), 0);
    close(client_sock);
    return;
}