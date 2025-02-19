#include "server/server.h"

void hello_world(request* req, response* res) {
    send_file(res, "/index.html");
}

void template_test(request* req, response* res) {
    send_template(res, "./files/template.html", "This is an awesome demonstration of my template engine!<br>"
    " None of this was actually written in HTML");
}

void json_test(request* req, response* res) {
    JSON* obj = malloc(sizeof(JSON));
    if (!obj) {
        printf("bruh\n");
    }

    JSON* inner = malloc(sizeof(JSON));
    insert(inner, STRING, "inner key", "inner value");
    int inner_num = 2;
    insert(inner, INT, "inner int", &inner_num);
    insert(obj, OBJECT, "object key", inner);

    int num = 1;
    double db = 3.14159265359;
    insert(obj, STRING, "json key", "json value");
    insert(obj, INT, "int test", &num);

    list_t* list = malloc(sizeof(list_t));
    list->size = 0;
    list_insert(list, STRING, "string");
    list_insert(list, INT, &num);
    list_insert(list, DOUBLE, &db);
    char* list_string = list_stringify(list);
    insert(obj, LIST, "list", list);

    char* string = stringify(obj);
    send_json(res, string);
}

int main() {
    server* server = init();
    register_route("/", GET, hello_world);
    register_route("/", POST, hello_world);
    register_route("/template", GET, template_test);
    register_static("/css");
    register_route("/json", GET, json_test);
    start_server(server);

    return 0;
}