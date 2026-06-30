#ifndef CLI_H
#define CLI_H

#include "../p1_tableau/tableau.h"

/* Lance la boucle interactive (REPL) : lit, parse et execute les commandes
 * SET, GET, DEL, LPUSH, RPUSH, LPOP, RPOP, EXIT sur la table fournie. */
void run_cli(HashTable *ht);

/* Suite de tests automatiques (assert). Cree et detruit sa propre table,
 * n'interfere donc pas avec une session interactive en cours. */
void run_tests(void);

/* Mesure le temps moyen de SET/GET a differentes echelles (N croissant)
 * pour verifier que la complexite reste O(1) amortie. */
void run_benchmark(void);

#endif
