#ifndef SERVER_H
#define SERVER_H

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
#include "request/request.h"
#include "response/response.h"

#define PORT 8080
#define HOST "127.0.0.1"
#define BUFFER_SIZE 4096

/**
 * Handle a request from a client on the given
 * socket number. Calls functions from 
 * the "request" and "response" modules to get
 * this done.
 */
void handle_request(int client_sock);

/**
 * Closes the server and exits the program.
 * 
 * This is done when the user hits CTRL + C, preventing
 * a "socket in use" error if we restart the server too quickly.
 */
void close_server();

/**
 * This is the main function of this module.
 * 
 * It starts the server and begins listening for requests.
 */
int serve();
#endif