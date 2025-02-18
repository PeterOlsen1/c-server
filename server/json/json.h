#ifndef JSON_H
#define JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define JSON_MAX_LENGTH 150
#define MAX_HASH_POWER 3
#define MAX_JSON_STRING_LENGTH 4096

/**
 * Define enum type so we can keep
 * track of what each entry in an object is
 */
typedef enum {
    STRING,
    INT,
    DOUBLE,
    LIST,
    OBJECT
} JSON_Type;

//worry about implementing lists in JSON later
typedef struct {
    void* value;
    JSON_Type type;
    struct node_t* next;
} node_t;

typedef struct {
    int size;
    node_t* head;
} list_t;

/**
 * A singular JSON entry
 * 
 * Keeps track of the type, key, and value
 * so that we can get that information when traversing the
 * object later
 */
typedef struct JSON_Entry {
    JSON_Type type;
    struct JSON_Entry* next;
    char* key;
    void* value;
} JSON_Entry;

typedef struct {
    JSON_Entry* array[JSON_MAX_LENGTH];
} JSON;

int insert(JSON* obj, JSON_Type type, char* key, void* value);

char* stringify(JSON* obj);

char* list_stringify(list_t* list);

void free_json(JSON* obj);

#endif // JSON_H