#include "server/server.h"

void hello_world(request* req, response* res) {
    send_file(res, "/index.html");
}

void template_test(request* req, response* res) {
    send_template(res, "./files/template.html", "This is an awesome demonstration of my template engine!<br>"
    " None of this was actually written in HTML");
}

int main() {
    server* server = init();
    register_route("/", GET, hello_world);
    register_route("/template", GET, template_test);
    register_static("/css");
    start_server(server);

    return 0;
}