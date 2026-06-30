#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "cli.h"
#include "../p1_tableau/tableau.h"
#include "../p2_hashage/hash.h"
#include "../p3_collisions/liste_simple.h"
#include "../p4_liste/liste_double.h"

/* ============================================================
 * Helpers internes
 *
 * db_set/db_get/db_del (P3) ne gerent QUE le type STRING.
 * Pour LPUSH/RPUSH/LPOP/RPOP il n'existe pas d'equivalent
 * "list-aware" fourni par un autre module : c'est au CLI (P5),
 * qui voit tous les headers, de chercher/creer l'Entry de type
 * TYPE_LIST directement dans la table de hachage.
 * ============================================================ */

/* Recherche en lecture seule : ne modifie jamais la table. */
static Entry* find_entry(HashTable *ht, const char *key) {
    unsigned int index = hash(key, ht->size);
    Entry *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) return current;
        current = current->next;
    }
    return NULL;
}

/* Pour LPUSH/RPUSH : renvoie l'Entry de type LIST associee a key,
 * en la creant si besoin. Si la cle existe deja avec un type
 * STRING, met *wrong_type a 1 et renvoie NULL. */
static Entry* get_or_create_list_entry(HashTable *ht, const char *key, int *wrong_type) {
    *wrong_type = 0;

    Entry *e = find_entry(ht, key);
    if (e != NULL) {
        if (e->type != TYPE_LIST) {
            *wrong_type = 1;
            return NULL;
        }
        return e;
    }

    unsigned int index = hash(key, ht->size);
    Entry *new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->type = TYPE_LIST;
    new_entry->str_value = NULL;
    new_entry->list_value = list_create();
    new_entry->next = ht->buckets[index];
    ht->buckets[index] = new_entry;
    ht->count++;

    /* db_set verifie ce ratio apres chaque SET (cote P3) ; comme on
     * insere ici en dehors de db_set, on doit refaire la verification
     * nous-memes pour rester coherent avec le reste du systeme. */
    if ((float) ht->count / ht->size > 0.7) {
        ht_resize(ht);
    }

    /* Filet de securite : on re-cherche par cle plutot que de
     * supposer que le pointeur new_entry reste valide apres un
     * eventuel resize cote P1. */
    return find_entry(ht, key);
}

static void print_usage_error(const char *usage) {
    printf("(error) ERR usage: %s\n", usage);
}

/* Vrai s'il reste un token apres ceux deja consommes par strtok :
 * sert a detecter "trop d'arguments". */
static int has_extra_args(void) {
    return strtok(NULL, " ") != NULL;
}

#define WRONGTYPE_MSG "(error) WRONGTYPE Operation against a key holding the wrong kind of value\n"

/* ============================================================
 * Un handler par commande
 * ============================================================ */

static void handle_set(HashTable *ht, char *key, char *value) {
    if (!key || !value || has_extra_args()) {
        print_usage_error("SET key value");
        return;
    }
    db_set(ht, key, value);
    printf("OK\n");
}

static void handle_get(HashTable *ht, char *key) {
    if (!key || has_extra_args()) {
        print_usage_error("GET key");
        return;
    }
    Entry *e = find_entry(ht, key);
    if (!e) {
        printf("(nil)\n");
    } else if (e->type != TYPE_STRING) {
        printf(WRONGTYPE_MSG);
    } else {
        printf("%s\n", db_get(ht, key));
    }
}

static void handle_del(HashTable *ht, char *key) {
    if (!key || has_extra_args()) {
        print_usage_error("DEL key");
        return;
    }
    db_del(ht, key);
    printf("OK\n");
}

static void handle_push(HashTable *ht, char *key, char *value, int push_left) {
    if (!key || !value || has_extra_args()) {
        print_usage_error(push_left ? "LPUSH key value" : "RPUSH key value");
        return;
    }
    int wrong_type = 0;
    Entry *e = get_or_create_list_entry(ht, key, &wrong_type);
    if (wrong_type) {
        printf(WRONGTYPE_MSG);
        return;
    }
    ListeDouble *list = (ListeDouble*) e->list_value;
    if (push_left) list_lpush(list, value);
    else            list_rpush(list, value);
    printf("OK (length: %d)\n", list->length);
}

static void handle_pop(HashTable *ht, char *key, int pop_left) {
    if (!key || has_extra_args()) {
        print_usage_error(pop_left ? "LPOP key" : "RPOP key");
        return;
    }
    Entry *e = find_entry(ht, key);
    if (!e) {
        printf("(nil)\n");
        return;
    }
    if (e->type != TYPE_LIST) {
        printf(WRONGTYPE_MSG);
        return;
    }
    ListeDouble *list = (ListeDouble*) e->list_value;
    char *val = pop_left ? list_lpop(list) : list_rpop(list);
    if (!val) {
        printf("(nil)\n");
    } else {
        printf("%s\n", val);
        free(val); /* list_lpop/list_rpop transferent la propriete de la valeur a l'appelant */
    }
}

/* ============================================================
 * REPL
 * ============================================================ */

void run_cli(HashTable *ht) {
    printf("alpha-Redis -- tapez EXIT pour quitter\n");
    char line[1024];

    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = 0;

        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, " ");
            handle_set(ht, key, val);
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            handle_get(ht, key);
        } else if (strcmp(cmd, "DEL") == 0) {
            char *key = strtok(NULL, " ");
            handle_del(ht, key);
        } else if (strcmp(cmd, "LPUSH") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, " ");
            handle_push(ht, key, val, 1);
        } else if (strcmp(cmd, "RPUSH") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, " ");
            handle_push(ht, key, val, 0);
        } else if (strcmp(cmd, "LPOP") == 0) {
            char *key = strtok(NULL, " ");
            handle_pop(ht, key, 1);
        } else if (strcmp(cmd, "RPOP") == 0) {
            char *key = strtok(NULL, " ");
            handle_pop(ht, key, 0);
        } else if (strcmp(cmd, "EXIT") == 0) {
            printf("Bye!\n");
            break;
        } else {
            printf("Commande inconnue: %s\n", cmd);
        }
    }
}

/* ============================================================
 * Tests d'integration
 * ============================================================ */

void run_tests(void) {
    printf("=== Tests P5 : validation du systeme ===\n\n");
    HashTable *ht = ht_create(16);
    assert(ht != NULL);

    printf("--- SET/GET ---\n");
    db_set(ht, "name", "Alice");
    assert(strcmp(db_get(ht, "name"), "Alice") == 0);
    printf("OK\n\n");

    printf("--- DEL + cle inexistante ---\n");
    db_del(ht, "name");
    assert(db_get(ht, "name") == NULL);
    assert(db_get(ht, "ghost") == NULL);
    printf("OK\n\n");

    printf("--- LPUSH/RPUSH/LPOP/RPOP ---\n");
    int wt = 0;
    Entry *e = get_or_create_list_entry(ht, "fruits", &wt);
    assert(e != NULL && wt == 0);
    ListeDouble *list = (ListeDouble*) e->list_value;
    list_lpush(list, "pomme");
    list_rpush(list, "banane");
    assert(list->length == 2);
    char *v = list_lpop(list);
    assert(strcmp(v, "pomme") == 0);
    free(v);
    v = list_rpop(list);
    assert(strcmp(v, "banane") == 0);
    free(v);
    assert(list->length == 0);
    assert(list_lpop(list) == NULL); /* liste vide -> NULL, pas de crash */
    printf("OK\n\n");

    printf("--- Mauvais type ---\n");
    wt = 0;
    get_or_create_list_entry(ht, "mylist", &wt);
    assert(wt == 0);
    Entry *bad = find_entry(ht, "mylist");
    assert(bad != NULL && bad->type == TYPE_LIST);
    /* GET sur "mylist" ou LPUSH sur "name" (apres un SET) declenchent
     * WRONGTYPE dans le CLI -- verifie ici au niveau des types bruts. */
    db_set(ht, "scalaire", "valeur");
    Entry *s = find_entry(ht, "scalaire");
    assert(s->type == TYPE_STRING);
    printf("OK\n\n");

    printf("--- Collisions / volume (1000 cles) ---\n");
    for (int i = 0; i < 1000; i++) {
        char key[20], val[20];
        sprintf(key, "key%d", i);
        sprintf(val, "val%d", i);
        db_set(ht, key, val);
    }
    for (int i = 0; i < 1000; i++) {
        char key[20], expected[20];
        sprintf(key, "key%d", i);
        sprintf(expected, "val%d", i);
        assert(strcmp(db_get(ht, key), expected) == 0);
    }
    printf("OK (aucune cle perdue, count=%d, size=%d)\n\n", ht->count, ht->size);

    ht_destroy(ht);
    printf("=== Tous les tests sont passes ===\n");
}

/* ============================================================
 * Benchmark O(1)
 * ============================================================ */

void run_benchmark(void) {
    printf("\n=== Benchmark : SET/GET a differentes echelles ===\n\n");
    int sizes[] = {1000, 10000, 100000};
    int nb = sizeof(sizes) / sizeof(sizes[0]);

    for (int s = 0; s < nb; s++) {
        int n = sizes[s];
        HashTable *ht = ht_create(16);

        clock_t start = clock();
        for (int i = 0; i < n; i++) {
            char key[20], val[20];
            sprintf(key, "k%d", i);
            sprintf(val, "v%d", i);
            db_set(ht, key, val);
        }
        clock_t after_set = clock();

        for (int i = 0; i < n; i++) {
            char key[20];
            sprintf(key, "k%d", i);
            db_get(ht, key);
        }
        clock_t after_get = clock();

        double set_total = (double)(after_set - start) / CLOCKS_PER_SEC;
        double get_total = (double)(after_get - after_set) / CLOCKS_PER_SEC;

        printf("N = %7d | SET moyen : %.6f ms/op | GET moyen : %.6f ms/op\n",
               n, (set_total / n) * 1000.0, (get_total / n) * 1000.0);

        ht_destroy(ht);
    }

    printf("\nSi le temps moyen par operation reste stable quand N augmente,\n");
    printf("la complexite O(1) amortie est validee.\n");
}
