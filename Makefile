CC = gcc
CFLAGS = -Wall -Wextra -Irequest

all: clean run

run: clean server
	@./server

server: index.o request/request.o response/response.o
	@$(CC) $(CFLAGS) -o server index.o request/request.o response/response.o

index.o: index.c request/request.h response/response.h
	@$(CC) $(CFLAGS) -c index.c

request/request.o: request/request.c request/request.h
	@$(CC) $(CFLAGS) -c request/request.c -o request/request.o

response/response.o: response/response.c response/response.h
	@$(CC) $(CFLAGS) -c response/response.c -o response/response.o


# testing scripts
test_request: clean
	@$(CC) request/request.c -o test_request
	@./test_request
	@rm -f test_request

test_response: clean
	@$(CC) response/response.c -o test_response
	@./test_response
	@rm -f test_response


clean:
	@rm -f *.o request/*.o response/*.o server