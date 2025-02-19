CC = gcc
CFLAGS = -Wall -Wextra -Irequest
LDFLAGS = -lm

# variables
SRC_DIR = server
RESPONSE_DIR = $(SRC_DIR)/response
REQUEST_DIR = $(SRC_DIR)/request
JSON_DIR = $(SRC_DIR)/json

OBJ = index.o \
      $(SRC_DIR)/server.o \
      $(RESPONSE_DIR)/response.o \
      $(REQUEST_DIR)/request.o \
      $(JSON_DIR)/json.o

TARGET = index

run: $(TARGET)
	./$(TARGET)
	
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)


# $< refers to the c file
# $@ refers to the .o file
# this means that all .o files depend on their .c file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

json: $(JSON_DIR)/json.o
	$(CC) $(CFLAGS) $< -o json
	./json

clean:
	@rm -f $(OBJ) $(TARGET) json
