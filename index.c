#include "server/server.h"

void hello_world(request* req) {
    send_file(req->socket, "/index.html");
}

int main() {
    server* server = init();
    register_route("/", hello_world);
    start_server(server);

    return 0;
}