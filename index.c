#include "server/server.h"

void hello_world(request* req, response* res) {
    send_file(res, "/index.html");
}

int main() {
    server* server = init();
    register_route("/", hello_world);
    start_server(server);

    return 0;
}