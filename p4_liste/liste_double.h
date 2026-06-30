#ifndef LISTE_DOUBLE_H
#define LISTE_DOUBLE_H

/* ============================================================
 *  P4 — Liste Doublement Chainee
 *  Projet alpha-Redis
 *  Support du type de donnee LIST (LPUSH / RPUSH / LPOP / RPOP)
 *  Toutes les operations sont en O(1)
 * ============================================================ */

/* Un noeud de la liste */
typedef struct Node {
    char        *value;   /* La valeur stockee (chaine de caracteres) */
    struct Node *prev;    /* Pointeur vers le noeud precedent          */
    struct Node *next;    /* Pointeur vers le noeud suivant            */
} Node;

/* La liste avec acces direct aux deux extremites */
typedef struct {
    Node *head;           /* Pointeur vers le premier element (tete)   */
    Node *tail;           /* Pointeur vers le dernier element (queue)   */
    int   length;         /* Nombre d'elements dans la liste            */
} ListeDouble;

/* ------ Fonctions publiques ------ */

/* Cree et retourne une liste vide */
ListeDouble* list_create(void);

/* Insere une valeur en TETE de liste — O(1) */
void  list_lpush(ListeDouble *list, const char *value);

/* Insere une valeur en QUEUE de liste — O(1) */
void  list_rpush(ListeDouble *list, const char *value);

/* Extrait et retourne la valeur en TETE — O(1)
 * Retourne NULL si la liste est vide
 * ATTENTION : l'appelant est responsable de free() sur le retour */
char* list_lpop(ListeDouble *list);

/* Extrait et retourne la valeur en QUEUE — O(1)
 * Retourne NULL si la liste est vide
 * ATTENTION : l'appelant est responsable de free() sur le retour */
char* list_rpop(ListeDouble *list);

/* Libere toute la memoire allouee par la liste */
void  list_destroy(ListeDouble *list);

#endif /* LISTE_DOUBLE_H */
