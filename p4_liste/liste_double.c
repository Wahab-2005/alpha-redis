#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "liste_double.h"

/* ============================================================
 *  P4 — Implementation de la Liste Doublement Chainee
 *  Projet alpha-Redis
 * ============================================================ */


/* ------------------------------------------------------------
 * list_create : Cree une liste vide
 * ------------------------------------------------------------ */
ListeDouble* list_create(void) {
    ListeDouble *list = malloc(sizeof(ListeDouble));
    if (!list) {
        fprintf(stderr, "Erreur : malloc liste_double echoue\n");
        exit(EXIT_FAILURE);
    }
    list->head   = NULL;
    list->tail   = NULL;
    list->length = 0;
    return list;
}


/* ------------------------------------------------------------
 * list_lpush : Insere une valeur en TETE — O(1)
 *
 * Avant : head -> [A] <-> [B] <- tail
 * Apres : head -> [NEW] <-> [A] <-> [B] <- tail
 * ------------------------------------------------------------ */
void list_lpush(ListeDouble *list, const char *value) {
    if (!list || !value) return;

    Node *new_node   = malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Erreur : malloc node echoue\n");
        exit(EXIT_FAILURE);
    }
    new_node->value = strdup(value);
    new_node->prev  = NULL;          /* Aucun precedent : c'est la nouvelle tete */
    new_node->next  = list->head;    /* L'ancienne tete devient le suivant        */

    if (list->head != NULL) {
        /* La liste n'est pas vide : relier l'ancienne tete en arriere */
        list->head->prev = new_node;
    } else {
        /* Liste etait vide : le nouveau noeud est aussi la queue */
        list->tail = new_node;
    }

    list->head = new_node;
    list->length++;
}


/* ------------------------------------------------------------
 * list_rpush : Insere une valeur en QUEUE — O(1)
 *
 * Avant : head -> [A] <-> [B] <- tail
 * Apres : head -> [A] <-> [B] <-> [NEW] <- tail
 * ------------------------------------------------------------ */
void list_rpush(ListeDouble *list, const char *value) {
    if (!list || !value) return;

    Node *new_node   = malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Erreur : malloc node echoue\n");
        exit(EXIT_FAILURE);
    }
    new_node->value = strdup(value);
    new_node->next  = NULL;          /* Aucun suivant : c'est la nouvelle queue  */
    new_node->prev  = list->tail;    /* L'ancienne queue devient le precedent    */

    if (list->tail != NULL) {
        /* La liste n'est pas vide : relier l'ancienne queue en avant */
        list->tail->next = new_node;
    } else {
        /* Liste etait vide : le nouveau noeud est aussi la tete */
        list->head = new_node;
    }

    list->tail = new_node;
    list->length++;
}


/* ------------------------------------------------------------
 * list_lpop : Extrait et retourne la valeur en TETE — O(1)
 *
 * Retourne NULL si la liste est vide.
 * L'appelant DOIT appeler free() sur la valeur retournee.
 * ------------------------------------------------------------ */
char* list_lpop(ListeDouble *list) {
    if (!list || list->head == NULL) return NULL;

    Node *to_remove = list->head;
    char *value     = to_remove->value;   /* On garde la valeur a retourner */

    list->head = to_remove->next;

    if (list->head != NULL) {
        /* Il reste des elements : la nouvelle tete n'a plus de precedent */
        list->head->prev = NULL;
    } else {
        /* La liste est maintenant vide : remettre tail a NULL aussi */
        list->tail = NULL;
    }

    free(to_remove);        /* Liberer le noeud (PAS la valeur : on la retourne) */
    list->length--;
    return value;           /* L'appelant est responsable du free() de 'value'   */
}


/* ------------------------------------------------------------
 * list_rpop : Extrait et retourne la valeur en QUEUE — O(1)
 *
 * Retourne NULL si la liste est vide.
 * L'appelant DOIT appeler free() sur la valeur retournee.
 * ------------------------------------------------------------ */
char* list_rpop(ListeDouble *list) {
    if (!list || list->tail == NULL) return NULL;

    Node *to_remove = list->tail;
    char *value     = to_remove->value;   /* On garde la valeur a retourner */

    list->tail = to_remove->prev;

    if (list->tail != NULL) {
        /* Il reste des elements : la nouvelle queue n'a plus de suivant */
        list->tail->next = NULL;
    } else {
        /* La liste est maintenant vide : remettre head a NULL aussi */
        list->head = NULL;
    }

    free(to_remove);        /* Liberer le noeud (PAS la valeur : on la retourne) */
    list->length--;
    return value;           /* L'appelant est responsable du free() de 'value'   */
}


/* ------------------------------------------------------------
 * list_destroy : Libere toute la memoire de la liste
 * ------------------------------------------------------------ */
void list_destroy(ListeDouble *list) {
    if (!list) return;

    Node *current = list->head;
    while (current != NULL) {
        Node *next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
    free(list);
}


/* ============================================================
 *  TESTS AUTOMATIQUES — executes uniquement si compile avec -DRUN_TESTS
 *  Commande : gcc -DRUN_TESTS -Wall -g -o test_p4 liste_double.c && ./test_p4
 * ============================================================ */
#ifdef RUN_TESTS

static void test_liste_double(void) {
    printf("==============================\n");
    printf(" Tests P4 — ListeDouble\n");
    printf("==============================\n");

    /* --- Test 1 : Creation d'une liste vide --- */
    ListeDouble *list = list_create();
    assert(list != NULL);
    assert(list->length == 0);
    assert(list->head   == NULL);
    assert(list->tail   == NULL);
    printf("[OK] list_create : liste vide\n");

    /* --- Test 2 : LPOP sur liste vide retourne NULL --- */
    char *val = list_lpop(list);
    assert(val == NULL);
    val = list_rpop(list);
    assert(val == NULL);
    printf("[OK] lpop/rpop sur liste vide -> NULL\n");

    /* --- Test 3 : LPUSH simple --- */
    list_lpush(list, "Alice");
    assert(list->length == 1);
    assert(strcmp(list->head->value, "Alice") == 0);
    assert(strcmp(list->tail->value, "Alice") == 0);
    assert(list->head->prev == NULL);
    assert(list->head->next == NULL);
    printf("[OK] lpush premier element (head == tail)\n");

    /* --- Test 4 : LPUSH multiple --- */
    list_lpush(list, "Bob");
    list_lpush(list, "Charlie");
    /* Etat attendu : head -> [Charlie] <-> [Bob] <-> [Alice] <- tail */
    assert(list->length == 3);
    assert(strcmp(list->head->value, "Charlie") == 0);
    assert(strcmp(list->tail->value, "Alice")   == 0);
    assert(list->head->prev == NULL);
    assert(list->tail->next == NULL);
    assert(strcmp(list->head->next->value, "Bob") == 0);
    assert(strcmp(list->tail->prev->value, "Bob") == 0);
    printf("[OK] lpush multiple : head=Charlie, tail=Alice\n");

    /* --- Test 5 : RPUSH --- */
    list_rpush(list, "David");
    /* Etat : [Charlie] <-> [Bob] <-> [Alice] <-> [David] */
    assert(list->length == 4);
    assert(strcmp(list->tail->value, "David") == 0);
    assert(strcmp(list->tail->prev->value, "Alice") == 0);
    assert(list->tail->next == NULL);
    printf("[OK] rpush : tail=David\n");

    /* --- Test 6 : LPOP --- */
    val = list_lpop(list);
    assert(val != NULL);
    assert(strcmp(val, "Charlie") == 0);
    free(val);
    assert(list->length == 3);
    assert(strcmp(list->head->value, "Bob") == 0);
    assert(list->head->prev == NULL);
    printf("[OK] lpop : retourne Charlie, head=Bob\n");

    /* --- Test 7 : RPOP --- */
    val = list_rpop(list);
    assert(val != NULL);
    assert(strcmp(val, "David") == 0);
    free(val);
    assert(list->length == 2);
    assert(strcmp(list->tail->value, "Alice") == 0);
    assert(list->tail->next == NULL);
    printf("[OK] rpop : retourne David, tail=Alice\n");

    /* --- Test 8 : Vider la liste completement --- */
    val = list_lpop(list);  assert(strcmp(val, "Bob")   == 0); free(val);
    val = list_lpop(list);  assert(strcmp(val, "Alice") == 0); free(val);
    assert(list->length == 0);
    assert(list->head   == NULL);
    assert(list->tail   == NULL);
    printf("[OK] liste videe : head=NULL, tail=NULL, length=0\n");

    /* --- Test 9 : RPUSH puis RPOP sur liste vide --- */
    list_rpush(list, "Solo");
    assert(list->length == 1);
    assert(list->head == list->tail);
    val = list_rpop(list);
    assert(strcmp(val, "Solo") == 0); free(val);
    assert(list->length == 0);
    assert(list->head == NULL);
    assert(list->tail == NULL);
    printf("[OK] rpush + rpop sur liste a 1 element\n");

    /* --- Test 10 : Stress test (1000 RPUSH + 1000 LPOP) --- */
    for (int i = 0; i < 1000; i++) {
        char buf[20];
        sprintf(buf, "item%d", i);
        list_rpush(list, buf);
    }
    assert(list->length == 1000);
    for (int i = 0; i < 1000; i++) {
        char expected[20];
        sprintf(expected, "item%d", i);
        val = list_lpop(list);
        assert(val != NULL);
        assert(strcmp(val, expected) == 0);
        free(val);
    }
    assert(list->length == 0);
    assert(list->head   == NULL);
    assert(list->tail   == NULL);
    printf("[OK] stress test : 1000 rpush + 1000 lpop dans l'ordre\n");

    /* Nettoyage */
    list_destroy(list);

    printf("==============================\n");
    printf(" Tous les tests P4 : PASSED\n");
    printf("==============================\n");
}

int main(void) {
    test_liste_double();
    return 0;
}

#endif /* RUN_TESTS */
