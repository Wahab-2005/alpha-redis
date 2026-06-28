#include <stdio.h>
#include <stdlib.h>
#include "tableau.h"
#include "../p2_hashage/hash.h"   /* fournie par P2 : hash(cle, taille) */

/*
 * ht_create — crée un tableau vide de taille "initial_size".
 * Toutes les cases (buckets) commencent vides (NULL).
 */
HashTable *ht_create(int initial_size) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        fprintf(stderr, "Erreur : allocation HashTable impossible\n");
        exit(1);
    }

    ht->buckets = calloc(initial_size, sizeof(Entry *)); /* calloc = malloc + met tout à NULL */
    if (ht->buckets == NULL) {
        fprintf(stderr, "Erreur : allocation buckets impossible\n");
        free(ht);
        exit(1);
    }

    ht->size = initial_size;
    ht->count = 0;

    return ht;
}

/*
 * ht_resize — agrandit le tableau quand il devient trop plein.
 * On double la taille, puis on replace CHAQUE clé existante,
 * car la fonction de hachage dépend de "size" (donc l'emplacement
 * de chaque clé change quand size change).
 */
void ht_resize(HashTable *ht) {
    int old_size = ht->size;
    Entry **old_buckets = ht->buckets;

    int new_size = old_size * 2;
    Entry **new_buckets = calloc(new_size, sizeof(Entry *));
    if (new_buckets == NULL) {
        fprintf(stderr, "Erreur : allocation lors du resize impossible\n");
        exit(1);
    }

    /* On parcourt toutes les anciennes cases */
    for (int i = 0; i < old_size; i++) {
        Entry *current = old_buckets[i];

        /* On parcourt la liste chaînée de collisions de cette case */
        while (current != NULL) {
            Entry *next = current->next; /* on garde le suivant avant de bouger "current" */

            /* Nouvel emplacement, calculé avec la NOUVELLE taille */
            unsigned long new_index = hash(current->key, new_size);

            /* On insère "current" en tête de la liste à new_index */
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next;
        }
    }

    free(old_buckets);
    ht->buckets = new_buckets;
    ht->size = new_size;
    /* ht->count ne change pas : on a juste déplacé les mêmes clés */
}

/*
 * ht_destroy — libère TOUTE la mémoire du tableau.
 * Pour chaque case, on libère toute la liste chaînée de collisions,
 * et pour chaque Entry, on libère sa clé + sa valeur.
 */
void ht_destroy(HashTable *ht) {
    if (ht == NULL) return;

    for (int i = 0; i < ht->size; i++) {
        Entry *current = ht->buckets[i];

        while (current != NULL) {
            Entry *next = current->next;

            free(current->key);

            if (current->type == TYPE_STRING) {
                free(current->str_value);
            } else if (current->type == TYPE_LIST) {
                /* La libération du contenu de la liste (P4) doit être
                 * faite par la fonction de destruction de p4_liste,
                 * appelée ici si elle est disponible :
                 * liste_double_destroy(current->list_value);
                 */
            }

            free(current);
            current = next;
        }
    }

    free(ht->buckets);
    free(ht);
}
