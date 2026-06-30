#include <stdio.h>
#include <string.h>
#include "p1_tableau/tableau.h"
#include "p5_cli/cli.h"

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        run_tests();
        run_benchmark();
        return 0;
    }

    HashTable *ht = ht_create(16);
    if (!ht) {
        fprintf(stderr, "Erreur : impossible de creer la table de hachage.\n");
        return 1;
    }

    run_cli(ht);

    ht_destroy(ht);
    return 0;
}
