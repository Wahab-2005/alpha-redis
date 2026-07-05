/*
 * test_p3.c — Programme de test pour le module P3 (liste_simple)
 *
 * Teste SET, GET, DEL et la gestion des collisions.
 * A placer a la racine du projet (alpha-redis/).
 *
 * Compilation :
 *   gcc test_p3.c p1_tableau/tableau.c p2_hashage/hash.c p3_collisions/liste_simple.c -I. -o test_p3
 *
 * Execution :
 *   ./test_p3   (ou test_p3.exe sous Windows)
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "p1_tableau/tableau.h"
#include "p3_collisions/liste_simple.h"

int tests_passed = 0;
int tests_failed = 0;

void check(int condition, const char *test_name) {
    if (condition) {
        printf("  [OK]   %s\n", test_name);
        tests_passed++;
    } else {
        printf("  [FAIL] %s\n", test_name);
        tests_failed++;
    }
}

int main(void) {
    printf("=== Tests du module P3 (liste_simple) ===\n\n");

    HashTable *ht = ht_create(8);

    /* --- Test 1 : SET puis GET --- */
    printf("Test 1 : SET / GET simple\n");
    db_set(ht, "name", "Alice");
    char *val = db_get(ht, "name");
    check(val != NULL && strcmp(val, "Alice") == 0, "GET retourne la bonne valeur");

    /* --- Test 2 : GET sur une cle inexistante --- */
    printf("\nTest 2 : GET sur cle inexistante\n");
    char *missing = db_get(ht, "inexistant");
    check(missing == NULL, "GET retourne NULL pour une cle absente");

    /* --- Test 3 : SET met a jour une cle existante --- */
    printf("\nTest 3 : SET met a jour\n");
    db_set(ht, "name", "Bob");
    val = db_get(ht, "name");
    check(val != NULL && strcmp(val, "Bob") == 0, "GET retourne la nouvelle valeur apres mise a jour");

    /* --- Test 4 : DEL supprime correctement --- */
    printf("\nTest 4 : DEL\n");
    db_del(ht, "name");
    val = db_get(ht, "name");
    check(val == NULL, "GET retourne NULL apres DEL");

    /* --- Test 5 : DEL sur une cle inexistante ne plante pas --- */
    printf("\nTest 5 : DEL sur cle inexistante\n");
    db_del(ht, "inexistant");
    check(1, "DEL sur cle inexistante ne fait pas crasher le programme");

    /* --- Test 6 : plusieurs cles differentes --- */
    printf("\nTest 6 : plusieurs cles\n");
    db_set(ht, "a", "1");
    db_set(ht, "b", "2");
    db_set(ht, "c", "3");
    check(strcmp(db_get(ht, "a"), "1") == 0 &&
          strcmp(db_get(ht, "b"), "2") == 0 &&
          strcmp(db_get(ht, "c"), "3") == 0,
          "Plusieurs cles cohabitent sans s'ecraser");

    /* --- Test 7 : beaucoup de cles (force des collisions + resize) --- */
    printf("\nTest 7 : insertion massive (200 cles, force collisions et resize)\n");
    int all_ok = 1;
    for (int i = 0; i < 200; i++) {
        char key[20], value[20];
        sprintf(key, "key%d", i);
        sprintf(value, "val%d", i);
        db_set(ht, key, value);
    }
    for (int i = 0; i < 200; i++) {
        char key[20], expected[20];
        sprintf(key, "key%d", i);
        sprintf(expected, "val%d", i);
        char *got = db_get(ht, key);
        if (got == NULL || strcmp(got, expected) != 0) {
            all_ok = 0;
            printf("       -> probleme avec %s (attendu '%s', recu '%s')\n",
                   key, expected, got ? got : "NULL");
        }
    }
    check(all_ok, "Toutes les 200 cles sont retrouvees correctement (aucune perte)");
    printf("       Taille finale du tableau apres resize : %d (count: %d)\n", ht->size, ht->count);

    /* --- Resume --- */
    printf("\n=== Resultat : %d reussis / %d echoues ===\n", tests_passed, tests_failed);

    ht_destroy(ht);

    return tests_failed == 0 ? 0 : 1;
}
