CC = gcc
CFLAGS = -Wall -Wextra -Irequest

all: server

run: clean server
	@./server

server: index.o request/request.o
	@$(CC) $(CFLAGS) -o server index.o request/request.o

test_request: request/request.o
	@$(CC) $(CFLAGS) -o test_request request/request.o request/test_request.c
	@./test_request

index.o: index.c request/request.h
	@$(CC) $(CFLAGS) -c index.c

request/request.o: request/request.c request/request.h
	@$(CC) $(CFLAGS) -c request/request.c -o request/request.o

clean:
	rm -f *.o request/*.o server