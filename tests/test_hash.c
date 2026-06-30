#include <stdio.h>
#include "../p1_tableau/tableau.h"
#include "../p2_hashage/hash.h"

#define TABLE_SIZE 16
#define NUM_KEYS 20

int main(void) {
    /* Compteur de collisions par bucket */
    int buckets[TABLE_SIZE] = {0};

    /* Quelques clés de test réalistes */
    const char *keys[NUM_KEYS] = {
        "username", "password", "email", "session_id", "user:1",
        "user:2", "user:3", "cart:42", "token_abc", "config:max",
        "config:min", "ip_address", "last_login", "role", "status",
        "counter", "score", "level", "lang", "theme"
    };

    printf("=== Test de distribution DJB2 (table size = %d) ===\n\n", TABLE_SIZE);

    for (int i = 0; i < NUM_KEYS; i++) {
        unsigned long idx = hash(keys[i], TABLE_SIZE);
        buckets[idx]++;
        printf("%-15s -> bucket %lu\n", keys[i], idx);
    }

    printf("\n=== Histogramme de repartition ===\n");
    int max_in_bucket = 0;
    int empty_buckets = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("bucket %2d [%2d] : ", i, buckets[i]);
        for (int j = 0; j < buckets[i]; j++) printf("#");
        printf("\n");

        if (buckets[i] > max_in_bucket) max_in_bucket = buckets[i];
        if (buckets[i] == 0) empty_buckets++;
    }

    printf("\n=== Resume ===\n");
    printf("Cles testees       : %d\n", NUM_KEYS);
    printf("Buckets vides      : %d / %d\n", empty_buckets, TABLE_SIZE);
    printf("Max cles/bucket    : %d\n", max_in_bucket);
    printf("Moyenne attendue   : %.2f\n", (double)NUM_KEYS / TABLE_SIZE);

    if (max_in_bucket <= 3) {
        printf("-> Distribution correcte, pas de cluster majeur.\n");
    } else {
        printf("-> Attention, cluster detecte sur un bucket.\n");
    }

    return 0;
}