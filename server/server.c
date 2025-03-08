#include "server.h"

int sock;
server* SERVER;
int server_running = 0;

server* init() {
    printf("Server size: %lu\n", sizeof(server));
    server* server = malloc(sizeof(*server));

    if (!server) {
        printf("Failed to allocate memory for server object\n");
        return NULL;
    }
    server->route_count = 0;
    server->static_route_count = 0;
    server->max_request_size = 4096;
    server->port = 8080;
    server->max_sockets = 10;
    server->base_directory = "public";

    SERVER = server;
    return server;
}


/**
 * Register a route with the server
 */
void register_route(char* path, char* method, route_handler handler) {
    if (SERVER->route_count >= MAX_ROUTES) {
        printf("Max routes reached\n");
        return;
    }

    //check if route has already been registered
    for (unsigned int i = 0; i < SERVER->route_count; i++) {
        if (!strcmp(SERVER->routes[i]->path, path) && !strcmp(SERVER->routes[i]->method, method)) {
            printf("Route \"%s\" with method \"%s\" already exists\n", path, method);
            return;
        }
    }

    //make object
    route* r = malloc(sizeof(*r));
    r->path = path;
    r->handler = handler;
    r->method = method;

    //add to server
    SERVER->routes[SERVER->route_count] = r;
    SERVER->route_count++;
    return;
}

/**
* Regsister a static route
*/
void register_static(char* path) {
    if (SERVER->static_route_count >= MAX_STATIC_ROUTES) {
        printf("Max static routes reached\n");
        return;
    }

    //check if route has already been registered
    for (unsigned int i = 0; i < SERVER->static_route_count; i++) {
        if (!strcmp(SERVER->static_routes[i]->path, path)) {
            printf("Route \"%s\" already exists\n", path);
            return;
        }
    }

    static_route* r = malloc(sizeof(static_route));
    r->path = path;

    //add to server
    SERVER->static_routes[SERVER->static_route_count] = r;
    SERVER->static_route_count++;
    return;
}


/**
 * Handle a request from a client. Called from the
 * server listening loop
 */
void* handle_request(void* client_sock_ptr) {
    int client_sock = *(int*)client_sock_ptr; // Cast to int pointer and dereference
    free(client_sock_ptr);

    // Read and parse request data
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    ssize_t bytes_read = read(client_sock, buffer, BUFFER_SIZE);
    if (bytes_read < 0) {
        perror("Failed to read from socket");
        close(client_sock);
        return NULL;
    }

    //create response object
    response* res = malloc(sizeof(response));
    if (!res) {
        perror("Failed to allocate memory for response");
        close(client_sock);
        return NULL;
    }
    res->client_sock = client_sock;

    // parse the request
    request* req = parse_request(buffer, client_sock);
    if (!req) {
        printf("Bad request!\n");
        send_error(res, BAD_REQUEST, "Bad request");
        close(client_sock);
        return NULL;
    }

    log_request(req);

    // find the route for this request
    int method_flag = 0;
    for (unsigned int i = 0; i < SERVER->route_count; i++) {
        route* r = SERVER->routes[i];
        if (!r || !r->path || !req->path) {
            continue;
        }

        if (strcmp(req->path, r->path) == 0) {
            //incorrect method, keep going and send 405 if we don't match
            if (strcmp(req->method, r->method)) {
                method_flag = 1;
                continue;
            }

            // Call the handler
            r->handler(req, res);
            free_request(req);
            free_response(res);
            close(client_sock);
            return NULL;
        }
    }

    // check for static file request. this may be a little flawed
    //since we should see if the "path" starts with the "sr->path"
    for (unsigned int i = 0; i < SERVER->static_route_count; i++) {
        static_route* sr = SERVER->static_routes[i];
        if (sr && sr->path && strstr(req->path, sr->path)) {
            send_file(res, req->path);
            free_request(req);
            free_response(res);
            close(client_sock);
            return NULL;
        }
    }

    // Handle errors
    if (method_flag) {
        send_error(res, METHOD_NOT_ALLOWED, "Method not allowed");
    } else {
        printf("Sending 404\n");
        send_404(res);
    }

    free_request(req);
    free_response(res);
    close(client_sock);
    return NULL;
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
 * 
 * This is where all of the socket overhead happens.
 * This is also where the main listening loop takes place
 */
int start_server() {
    if (server_running) {
        printf("Server is already running\n");
        return -1;
    }

    if (!SERVER) {
        printf("Server object is invalid\n");
        return -1;
    }

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
    if (SERVER->port) {
        socket_server.sin_port = htons(SERVER->port);
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
    if (SERVER->max_sockets) {
        listening = listen(sock, SERVER->max_sockets);
    } else {
        listening = listen(sock, 10);
    }

    if (listening < 0) {
        printf("Failed to listen on the socket:\n%s", strerror(errno));
        return -1;
    }

    //log that we're listening
    if (SERVER->port) {
        printf("Server is listening on %s:%d!\n", HOST, SERVER->port);
    } else {
        printf("Server is listening on %s:%d!\n", HOST, PORT);
    }

    server_running = 1;

    //enter listening loop
    while (1) {
        struct sockaddr_in client;
        int client_size = sizeof(client);
        int client_sock = accept(sock, (struct sockaddr*)&client, (socklen_t*)&client_size);

        if (client_sock < 0) {
            printf("Failed to accept client connection:\n%s", strerror(errno));
        }
        else {
            int* client_sock_ptr = malloc(sizeof(int)); //freed by the handle_request method
            *client_sock_ptr = client_sock;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handle_request, client_sock_ptr);
            pthread_detach(thread_id);
        }
    }

    return 0;
}
