/*
 * liste_simple.c — Personne 3
 *
 * Implementation de la liste chainee simple pour la gestion des collisions
 * et les commandes SET / GET / DEL.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liste_simple.h"

/* ─────────────────────────────────────────────
 * db_set : stocker ou mettre a jour une cle string
 * ───────────────────────────────────────────── */
void db_set(HashTable *ht, const char *key, const char *value) {

    /* 1. Calculer l'index du bucket (hash de P2 retourne deja un index valide) */
    unsigned long index = hash(key, ht->size);

    /* 2. Parcourir la liste chainee de ce bucket */
    Entry *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            /* Cle trouvee : on met a jour la valeur */
            free(current->str_value);
            current->str_value = strdup(value);
            current->type = TYPE_STRING;
            return; /* Fin, pas besoin d'ajouter un maillon */
        }
        current = current->next;
    }

    /* 3. Cle non trouvee : creer un nouveau maillon */
    Entry *new_entry = (Entry *)malloc(sizeof(Entry));
    if (new_entry == NULL) {
        fprintf(stderr, "Erreur: allocation memoire echouee\n");
        return;
    }

    new_entry->key        = strdup(key);
    new_entry->type       = TYPE_STRING;
    new_entry->str_value  = strdup(value);
    new_entry->list_value = NULL;

    /* Insertion en tete de la liste chainee (O(1)) */
    new_entry->next          = ht->buckets[index];
    ht->buckets[index]       = new_entry;
    ht->count++;

    /* 4. P2 fournit ht_check_resize : elle verifie le facteur de charge
     * et appelle ht_resize() elle-meme si besoin. */
    ht_check_resize(ht);
}

/* ─────────────────────────────────────────────
 * db_get : recuperer la valeur d'une cle string
 * ───────────────────────────────────────────── */
char* db_get(HashTable *ht, const char *key) {

    /* 1. Calculer l'index du bucket */
    unsigned long index = hash(key, ht->size);

    /* 2. Parcourir la liste chainee du bucket */
    Entry *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            /* Cle trouvee */
            if (current->type != TYPE_STRING) {
                fprintf(stderr, "Erreur: la cle '%s' contient une liste, pas une string\n", key);
                return NULL;
            }
            return current->str_value;
        }
        current = current->next;
    }

    /* Cle non trouvee */
    return NULL;
}

/* ─────────────────────────────────────────────
 * db_del : supprimer une cle et liberer la memoire
 * ───────────────────────────────────────────── */
void db_del(HashTable *ht, const char *key) {

    /* 1. Calculer l'index du bucket */
    unsigned long index = hash(key, ht->size);

    Entry *current  = ht->buckets[index];
    Entry *previous = NULL;

    /* 2. Parcourir la chaine pour trouver le maillon */
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {

            /* 3. Retirer le maillon de la chaine */
            if (previous == NULL) {
                /* Le maillon est en tete : la case pointe sur le suivant */
                ht->buckets[index] = current->next;
            } else {
                /* Le maillon est au milieu ou en queue */
                previous->next = current->next;
            }

            /* 4. Liberer la memoire du maillon */
            free(current->key);
            free(current->str_value);
            /* Note : si type LIST, P5 doit appeler list_destroy avant db_del */
            free(current);
            ht->count--;
            return;
        }
        previous = current;
        current  = current->next;
    }

    /* Cle non trouvee : on ne fait rien */
}

/* ─────────────────────────────────────────────
 * db_set_list : stocker une liste doublement chainee
 * ───────────────────────────────────────────── */
void db_set_list(HashTable *ht, const char *key, void *list) {

    unsigned long index = hash(key, ht->size);

    /* Verifier si la cle existe deja */
    Entry *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            /* Cle trouvee : mettre a jour */
            current->list_value = list;
            current->type       = TYPE_LIST;
            return;
        }
        current = current->next;
    }

    /* Cle non trouvee : creer un nouveau maillon */
    Entry *new_entry = (Entry *)malloc(sizeof(Entry));
    if (new_entry == NULL) {
        fprintf(stderr, "Erreur: allocation memoire echouee\n");
        return;
    }

    new_entry->key        = strdup(key);
    new_entry->type       = TYPE_LIST;
    new_entry->str_value  = NULL;
    new_entry->list_value = list;

    /* Insertion en tete */
    new_entry->next    = ht->buckets[index];
    ht->buckets[index] = new_entry;
    ht->count++;

    ht_check_resize(ht);
}

/* ─────────────────────────────────────────────
 * db_get_entry : recuperer l'entree complete
 * ───────────────────────────────────────────── */
Entry* db_get_entry(HashTable *ht, const char *key) {

    unsigned long index = hash(key, ht->size);

    Entry *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL; /* Cle non trouvee */
}
