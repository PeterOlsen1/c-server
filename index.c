#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <request.c>

#define PORT 3000
#define HOST "127.0.0.1"
#define BUFFER_SIZE 4096
int sock;

/**
 * Signal handler for closing the server
 */
void close_server(int sig) {
    printf("Closing server...\n");
    close(sock);
    exit(0);
}


/**
 * Start server and start accepting requests
 */
int main() {
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
    }

    //do some housekeeping stuff and set up our server
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(HOST);

    //bind socket to address
    int binded = bind(sock, (struct sockaddr*)&server, sizeof(server));
    if (binded < 0) {
        printf("Failed to bind to the socket:\n%s", strerror(errno));
    }

    //listen!!!!
    int listening = listen(sock, 10);
    if (listening < 0) {
        printf("Failed to listen on the socket:\n%s", strerror(errno));
    }

    printf("Server is listening on %s:%d!\n", HOST, PORT);


    //enter listening loop
    while (1) {
        struct sockaddr_in client;
        int client_size = sizeof(client);
        int client_sock = accept(sock, (struct sockaddr*)&client, (socklen_t*)&client_size);

        if (client_sock < 0) {
            printf("Failed to accept client connection:\n%s", strerror(errno));
        }
        else {
            //read the client request
            char buffer[BUFFER_SIZE];
            read(client_sock, buffer, 1024);

            printf("Request: %s\n", buffer);
        }
    }

    //make sure we close the socket!
    close(sock);
    return 0;
}
