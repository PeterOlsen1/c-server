#include "server.h"

int sock;



/**
 * Handle a request from a client. Called from the
 * server listening loop
 */
void handle_request(void* client_sock_ptr) {
    int client_sock = *(int*)client_sock_ptr;
    free(client_sock_ptr);

    char buffer[BUFFER_SIZE];
    read(client_sock, buffer, BUFFER_SIZE);
    request* req = parse_request(buffer, client_sock);

    log_request(req);

    //invalid request was already logged. return
    if (!req) {
        printf("bad!\n");
        return;
    }

    send_file(client_sock, req->path);

    free_request(req);
}


/**
 * Signal handler for closing the server
 */
void close_server() {
    printf("Closing server...\n");
    close(sock);
    exit(0);
}


/**
 * Start server and start accepting requests
 */
int start_server(server* server) {
    //close the server if the user stops the program
    signal(SIGINT, close_server);
    
    printf("Starting web server...\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Error establishing socket:\n%s", strerror(errno));
        return -1;
    }

    //set socket options so we can CTRL + C as many times as we want
    int options = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    if (options < 0) {
        printf("Failed to set socket options:\n%s", strerror(errno));
        return -1;
    }

    //do some housekeeping stuff and set up our server
    struct sockaddr_in socket_server;
    socket_server.sin_family = AF_INET;
    if (server->port) {
        socket_server.sin_port = htons(server->port);
    } else {
        socket_server.sin_port = htons(PORT);
    }
    socket_server.sin_addr.s_addr = inet_addr(HOST);

    //bind socket to address
    int binded = bind(sock, (struct sockaddr*)&socket_server, sizeof(socket_server));
    if (binded < 0) {
        printf("Failed to bind to the socket:\n%s", strerror(errno));
        return -1;
    }

    //listen!!!!
    int listening;
    if (server->max_sockets) {
        listening = listen(sock, server->max_sockets);
    } else {
        listening = listen(sock, 10);
    }

    if (listening < 0) {
        printf("Failed to listen on the socket:\n%s", strerror(errno));
        return -1;
    }

    //log that we're listening
    if (server->port) {
        printf("Server is listening on %s:%d!\n", HOST, server->port);
    } else {
        printf("Server is listening on %s:%d!\n", HOST, PORT);
    }


    //enter listening loop
    while (1) {
        struct sockaddr_in client;
        int client_size = sizeof(client);
        int client_sock = accept(sock, (struct sockaddr*)&client, (socklen_t*)&client_size);

        if (client_sock < 0) {
            printf("Failed to accept client connection:\n%s", strerror(errno));
        }
        else {
            int* client_sock_ptr = malloc(sizeof(int));
            *client_sock_ptr = client_sock;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handle_request, client_sock_ptr);
            pthread_detach(thread_id);
        }
    }

    return 0;
}
