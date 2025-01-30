#include "json.h"

// JSON* init_json() {
//     JSON* obj = malloc(sizeof(JSON));
//     return obj;
// }

int insert(JSON* obj, JSON_Type type, char* key, void* value) {
    if (!obj || !type || !key || !value) {
        return -1;
    }
    

    return 0;
}

int hash(char* key) {
    int len = strlen(key);
    int total = 0;
    double factor = 0;
    // double factor_increse = MAX_HASH_POWER / len;
    for (int i = 0; i < len; i++) {
        total += (int) pow(key[i], factor);

        if (total < 0) {
            total = 0;
        }

        factor += 0.2;
    }

    printf("%d\n", total);

    return total % JSON_MAX_LENGTH;
}

char* stringify(JSON* obj) {
    return "JK LOL!";
}

int main() {
    // JSON* obj = malloc(sizeof(JSON));

    int hash1 = hash("THIS IS ME HASHING HI HI HI HI");
    printf("Hash1: %d\n", hash1);

    int hash2 = hash("THIS IS ME HASHINGG");
    printf("Hash2: %d\n", hash2);
    return 0;
}