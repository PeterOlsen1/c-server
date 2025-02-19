#include "json.h"

// JSON* init_json() {
//     JSON* obj = malloc(sizeof(JSON));
//     return obj;
// }

/**
 * Hash the given key, always assumed that the key is a string.
 * 
 * Hash function = 
 * sum {i in key} int(key[i]) ^ (MAX_HASH_POWER * i / len)
 * 
 * The max power is limted to 3, since ascii characters can
 * get up to 255, 255^3 = a lot (don't wanna overflow the max integer)
 */
int hash(char* key) {
    int len = strlen(key);
    int total = 0;
    double factor = 0;
    double factor_increase = MAX_HASH_POWER / len;
    for (int i = 0; i < len; i++) {
        total += (int) pow(key[i], factor);

        if (total < 0) {
            total = 0;
        }

        factor += factor_increase;
    }

    return total % JSON_MAX_LENGTH;
}

int insert(JSON* obj, JSON_Type type, char* key, void* value) {
    if (!obj || !key || !value) {
        return -1;
    }
    
    int idx = hash(key);
    JSON_Entry* entry = obj->array[idx];

    //bucket is empty, initialize the node
    if (!entry) {
        JSON_Entry* new_entry = malloc(sizeof(JSON_Entry));
        new_entry->type = type;
        new_entry->key = key;
        new_entry->value = value;
        obj->array[idx] = new_entry;
        return 0;
    }
    //bucket is not empty. iterate!
    else {
        while (entry->next) {
            entry = entry->next;
        }

        JSON_Entry* new_entry = malloc(sizeof(JSON_Entry));
        new_entry->type = type;
        new_entry->key = key;
        new_entry->value = value;
        entry->next = new_entry;
        return 0;
    }
}

int list_insert(list_t* list, JSON_Type type, void* value) {
    if (!list || !value) {
        return -1;
    }

    node_t* node = list->head;
    if (!node) {
        node = malloc(sizeof(node_t));
        node->type = type;
        node->value = value;
        list->head = node;
        list->size = 1;
        return 0;
    }
    else {
        while (node->next) {
            node = node->next;
        }

        node_t* new_node = malloc(sizeof(node_t));
        new_node->type = type;
        new_node->value = value;
        node->next = new_node;
        list->size++;
        return 0;
    }
}

char* stringify(JSON* obj) {
    size_t size = MAX_JSON_STRING_LENGTH;
    char* out = malloc(MAX_JSON_STRING_LENGTH);
    if (!out) {
        return NULL;
    }
    strcpy(out, "{");
    
    for (int i = 0; i < JSON_MAX_LENGTH; i++) {
        if (obj->array[i]) {
            JSON_Entry* ptr = obj->array[i];
            
            while (ptr) {
                char* inner_string = NULL;
                switch (ptr->type) {
                    case STRING: {
                        //format the inner string
                        size_t inner_size = strlen(ptr->key) + strlen(ptr->value) + 9;
                        inner_string = malloc(inner_size);
                        snprintf(inner_string, inner_size, "\"%s\": \"%s\"", ptr->key, (char*)ptr->value);
                        break;
                    }
                    case INT: {
                        //get the size of the number
                        char* int_string = malloc(10);
                        snprintf(int_string, 10, "%d", *(int*)ptr->value);
                        size_t inner_size = strlen(ptr->key) + strlen(int_string) + 9;

                        //format the inner string
                        inner_string = malloc(inner_size);
                        snprintf(inner_string, inner_size, "\"%s\": %d", ptr->key, *(int*)ptr->value);
                        free(int_string);
                        break;
                    }
                    case DOUBLE: {
                        //get the size of the number
                        char* double_string = malloc(32);
                        snprintf(double_string, 32, "%lf", *(double*)ptr->value);
                        size_t inner_size = strlen(ptr->key) + strlen(double_string) + 9;

                        //format the inner string
                        inner_string = malloc(inner_size);
                        snprintf(inner_string, inner_size, "\"%s\": %f", ptr->key, *(double*)ptr->value);
                        free(double_string);
                        break;
                    }
                    case OBJECT: {
                        //recursively stringify the object
                        char* object_string = stringify((JSON*)ptr->value);
                        size_t inner_size = strlen(ptr->key) + strlen(object_string) + 9;

                        //format the inner string
                        inner_string = malloc(inner_size);
                        snprintf(inner_string, inner_size, "\"%s\": %s", ptr->key, object_string);
                        free(object_string);
                        break;
                    }
                    case LIST: {
                        //recursively stringify the list
                        char* list_string = list_stringify((list_t*)ptr->value);
                        size_t inner_size = strlen(ptr->key) + strlen(list_string) + 9;

                        //format the inner string
                        inner_string = malloc(inner_size);
                        snprintf(inner_string, inner_size, "\"%s\": %s", ptr->key, list_string);
                        free(list_string);
                        break;
                    }
                    default:
                        ptr = ptr->next;
                        continue;
                }
                //make sure we have enough memory
                if (strlen(out) + strlen(inner_string) > size) {
                    size *= 2;
                    out = realloc(out, size);
                    if (!out) {
                        printf("Error reallocating more memory to JSON object!\n");
                        return NULL;
                    }
                }
                //concatenate inner string
                strcat(out, inner_string);
                free(inner_string);
                strcat(out, ", ");
                ptr = ptr->next;
            }
        }
    }

    
    //close object and return (remove the extra space and comma)
    out[strlen(out) - 2] = '}';
    out[strlen(out) - 1] = '\0';
    return out;
}

char* list_stringify(list_t* list) {
    size_t size = MAX_JSON_STRING_LENGTH;
    char* out = malloc(MAX_JSON_STRING_LENGTH);
    if (!out) {
        return NULL;
    }
    strcpy(out, "[");

    node_t* ptr = list->head;
    while (ptr) {
        char* inner_string = NULL;
        switch (ptr->type) {
            case STRING: {
                //format the inner string
                size_t inner_size = strlen(ptr->value) + 3;
                inner_string = malloc(inner_size);
                snprintf(inner_string, inner_size, "\"%s\"", (char*)ptr->value);
                break;
            }
            case INT: {
                //get the size of the number
                char* int_string = malloc(10);
                snprintf(int_string, 10, "%d", *(int*)ptr->value);
                size_t inner_size = strlen(int_string) + 3;

                //format the inner string
                inner_string = malloc(inner_size);
                snprintf(inner_string, inner_size, "%d", *(int*)ptr->value);
                free(int_string);
                break;
            }
            case DOUBLE: {
                //get the size of the number
                char* double_string = malloc(32);
                snprintf(double_string, 32, "%lf", *(double*)ptr->value);
                size_t inner_size = strlen(double_string) + 3;

                //format the inner string
                inner_string = malloc(inner_size);
                snprintf(inner_string, inner_size, "%lf", *(double*)ptr->value);
                free(double_string);
                break;
            }
            case OBJECT: {
                //recursively stringify the object
                char* object_string = stringify((JSON*)ptr->value);
                size_t inner_size = strlen(object_string) + 3;

                //format the inner string
                inner_string = malloc(inner_size);
                snprintf(inner_string, inner_size, "%s", object_string);
                free(object_string);
                break;
            }
            case LIST: {
                //recursively stringify the list
                char* list_string = list_stringify((list_t*)ptr->value);
                size_t inner_size = strlen(list_string) + 3;

                //format the inner string
                inner_string = malloc(inner_size);
                snprintf(inner_string, inner_size, "%s", list_string);
                free(list_string);
                break;
            }
            default:
                ptr = ptr->next;
                continue;
        }
        //make sure we have enough memory
        if (strlen(out) + strlen(inner_string) > size) {
            size *= 2;
            out = realloc(out, size);
            if (!out) {
                printf("Error reallocating more memory to JSON object!\n");
                return NULL;
            }
        }
        //concatenate inner string
        strcat(out, inner_string);
        free(inner_string);
        strcat(out, ", ");
        ptr = ptr->next;
    }

    //close object and return (remove the extra space and comma)
    out[strlen(out) - 2] = ']';
    out[strlen(out) - 1] = '\0';
    return out;
}

int list_contains(list_t* list, char* key) {
    if (!list || !key) {
        return 0;
    }

    node_t* ptr = list->head;
    while (ptr) {
        if (!strcmp(ptr->value, key)) {
            return 1;
        }
        ptr = ptr->next;
    }

    return 0;
}

void* get(JSON* obj, char* key) {
    if (!obj || !key) {
        return NULL;
    }

    int idx = hash(key);
    JSON_Entry* entry = obj->array[idx];
    while (entry) {
        if (!strcmp(entry->key, key)) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

void free_json(JSON* obj) {
    for (int i = 0; i < JSON_MAX_LENGTH; i++) {
        JSON_Entry* ptr = obj->array[i];
        while (ptr) {
            JSON_Entry* temp = ptr;
            ptr = ptr->next;
            free(temp);
        }
    }
    free(obj);
}

// int main() {
//     JSON* obj = malloc(sizeof(JSON));
//     if (!obj) {
//         printf("bruh\n");
//     }

//     JSON* inner = malloc(sizeof(JSON));
//     insert(inner, STRING, "inner key", "inner value");
//     int inner_num = 2;
//     insert(inner, INT, "inner int", &inner_num);
//     insert(obj, OBJECT, "object key", inner);

//     int num = 1;
//     double db = 3.14159265359;
//     insert(obj, STRING, "json key", "json value");
//     insert(obj, INT, "int test", &num);

//     list_t* list = malloc(sizeof(list_t));
//     list->size = 0;
//     list_insert(list, STRING, "string");
//     list_insert(list, INT, &num);
//     list_insert(list, DOUBLE, &db);
//     char* list_string = list_stringify(list);
//     // printf("List Stringify: %s\n", list_string);
//     insert(obj, LIST, "list", list);

//     char* string = stringify(obj);
//     printf("Stringify: %s\n", string);
//     return 0;
// }