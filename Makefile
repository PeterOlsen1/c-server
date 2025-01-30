CC = gcc
CFLAGS = -Wall -Wextra -Irequest

all: run

run: clean server_compile
	@./index

server_compile: index.o server/server.o server/response/response.o server/request/request.o
	@$(CC) $(CFLAGS) -o index index.o server/server.o server/response/response.o server/request/request.o

index.o: index.c
	@$(CC) $(CFLAGS) -c index.c

server/server.o: server/server.c server/response/response.h server/request/request.h
	@$(CC) $(CFLAGS) -c server/server.c -o server/server.o

server/request/request.o: server/request/request.c server/request/request.h
	@$(CC) $(CFLAGS) -c server/request/request.c -o server/request/request.o

server/response/response.o: server/response/response.c server/response/response.h
	@$(CC) $(CFLAGS) -c server/response/response.c -o server/response/response.o


# testing scripts
# test_request: clean
# 	@$(CC) request/request.c -o test_request
# 	@./test_request
# 	@rm -f test_request

# test_response: clean
# 	@$(CC) response/response.c -o test_response
# 	@./test_response
# 	@rm -f test_response

json: server/json/json.c
	$(CC) $(CFLAGS) server/json/json.c -o json
	./json


clean:
	@rm -f *.o request/*.o response/*.o server/server.o index