# C-Server

I am practicing my c skills here. Don't expect anything monumental. There's probably a lot of security vulnerabilities (oops!)

### To start:
1. Create a file with a "server.h" include
2. Create a main that instantiates a server object, like so:
    ```c
    server* server = init()
    ```
3. Modify server properties such as port, max request size, etc. These parameters are laid out in the **server** struct def in *server.h*
4. Call the *start_server* method, and the server will begin accepting requests on localhost:8080

### To configure routes:
1. Define some function that accepts *request\*, response** as arguments.
2. Call the *register_route* function, with (char* route, f (request* -> void)) as the arguments
3. Include info about static routes here when i actually do them

### To run:
Idk figure out stuff with makeifles

### Example:
```c
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
```

TODO:
* Make sure body parsing does not take any extra characters with it
* Log requests properly to the console

* Static routes
* Differnet request types
  * parsing post requests (mentioned earlier)
  * allow for endpoint definition of what to do in different reuqest types
* JSON?
  * JSON module from scratch? (essentially hash table)
* Templates?