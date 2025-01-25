#include "server/server.h"

int main() {
    server* server = malloc(sizeof(server));

    server->port = 8080;
    server->max_sockets = 1;

    start_server(server);

    return 0;
}