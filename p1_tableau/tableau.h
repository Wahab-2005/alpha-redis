#ifndef TABLEAU_H
#define TABLEAU_H

/*
 * tableau.h — Personne 1
 *
 * Définit les structures de données partagées par tout le projet.
 * Ce fichier doit être stable avant que les autres modules ne commencent
 * (P2, P3, P4 en dépendent directement).
 */

/* Le type de valeur stockée */
typedef enum {
    TYPE_STRING,
    TYPE_LIST
} ValueType;

/* Une entrée dans la base de données (un maillon de la liste de collision) */
typedef struct Entry {
    char *key;                 /* La clé (ex: "username") */
    ValueType type;            /* STRING ou LIST */
    char *str_value;           /* Si type == TYPE_STRING */
    void *list_value;          /* Si type == TYPE_LIST (pointe vers une ListeDouble, voir p4) */
    struct Entry *next;        /* Maillon suivant dans la liste chaînée de collision */
} Entry;

/* Le tableau principal (la base de données) */
typedef struct {
    Entry **buckets;           /* Tableau de pointeurs vers des listes chaînées */
    int size;                  /* Nombre de cases actuelles */
    int count;                 /* Nombre total de clés stockées */
} HashTable;

/* Fonctions à implémenter dans tableau.c */
HashTable *ht_create(int initial_size);
void       ht_resize(HashTable *ht);
void       ht_destroy(HashTable *ht);

#endif /* TABLEAU_H */
