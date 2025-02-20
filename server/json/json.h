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

/**
 * Initialize a JSON object
 */
JSON* init_json();

/**
 * Insert value of type type into obj at key
 */
int insert(JSON* obj, JSON_Type type, char* key, void* value);

/**
 * Essentially the same as Object.keys() in JS
 * Return a dynamic list of all obeject keys
 */
list_t* keys(JSON* obj);

/**
 * Stringify a JSON object
 */
char* stringify(JSON* obj);

/**
 * Check if obj contains key
 */
int contains(JSON* obj, char* key);

/**
 * Stringify a dynamic list
 */
char* list_stringify(list_t* list);

/**
 * insert a value into a list
 */
int list_insert(list_t* list, JSON_Type type, void* value);

/**
 * Check if a list contains a given key
 * 
 * return 1 on success, 0 on failure
 */
int list_contains(list_t* list, char* key);

/**
 * Free a json object
 */
void free_json(JSON* obj);

/**
 * Free a dynamic list
 */
void free_list(list_t* list);

/**
 * Get a value from a JSON object at key
 * 
 * YOU MUST CAST THE RETURNED VALUE TO THE CORRECT TYPE
 * OR BAD THINGS WILL HAPPEN
 */
void* get(JSON* obj, char* key);

#endif // JSON_H