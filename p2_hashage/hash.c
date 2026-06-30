#include "hash.h"
#include <string.h>

/*
 * hash — implémentation DJB2.
 * Algorithme simple et rapide, bonne distribution pour des clés texte.
 * hash = 5381, puis pour chaque caractère : hash = hash * 33 + c
 */
unsigned long hash(const char *key, int size) {
    unsigned long h = 5381;
    int c;

    while ((c = (unsigned char)(*key++))) {
        h = ((h << 5) + h) + c; /* h * 33 + c */
    }

    return h % (unsigned long)size;
}

/*
 * ht_check_resize — calcule le facteur de charge (count / size)
 * et appelle ht_resize() si on dépasse LOAD_FACTOR_THRESHOLD.
 */
void ht_check_resize(HashTable *ht) {
    if (ht == NULL) return;

    double load_factor = (double)ht->count / (double)ht->size;

    if (load_factor > LOAD_FACTOR_THRESHOLD) {
        ht_resize(ht);
    }
}