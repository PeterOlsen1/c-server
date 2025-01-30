#include "server/server.h"

void hello_world(request* req, response* res) {
    send_file(res, "/index.html");
}

void template_test(request* req, response* res) {
    send_template(res, "./files/template.html", "Hello", "World");
}

int main() {
    server* server = init();
    register_route("/", GET, hello_world);
    register_static("/css");
    register_route("/template", GET, template_test);
    start_server(server);

    return 0;
}