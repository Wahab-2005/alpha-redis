/* Fichier TEMPORAIRE, juste pour tester en attendant le vrai code de P2.
   A SUPPRIMER quand P2 aura livré son vrai hash.c */
#include "hash.h"

unsigned long hash(const char *key, int size) {
    unsigned long h = 5381;
    while (*key) {
        h = h * 33 + (unsigned char)(*key);
        key++;
    }
    return h % size;
}
