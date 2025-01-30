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
    // printf("%d\n", idx);
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
                        snprintf(inner_string, inner_size, "\"%s\":\"%s\"", ptr->key, (char*)ptr->value);
                        break;
                    }
                    case INT: {
                        //get the size of the number
                        char* int_string = malloc(10);
                        snprintf(int_string, 10, "%d", *(int*)ptr->value);
                        size_t inner_size = strlen(ptr->key) + strlen(int_string) + 9;

                        //format the inner string
                        inner_string = malloc(inner_size);
                        snprintf(inner_string, inner_size, "\"%s\":%d", ptr->key, *(int*)ptr->value);
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
                        snprintf(inner_string, inner_size, "\"%s\":%f", ptr->key, *(double*)ptr->value);
                        free(double_string);
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
                strcat(out, ",");
                ptr = ptr->next;
            }
        }
    }


    //close object and return
    out[strlen(out) - 1] = '}';
    return out;
}

int main() {
    JSON* obj = malloc(sizeof(JSON));
    if (!obj) {
        printf("bruh\n");
    }

    int num = 1;
    insert(obj, STRING, "json key", "json value");
    insert(obj, INT, "int test", &num);
    char* string = stringify(obj);
    printf("Stringify: %s\n", string);
    return 0;
}