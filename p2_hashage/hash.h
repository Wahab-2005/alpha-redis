#ifndef HASH_H
#define HASH_H

#include "../p1_tableau/tableau.h"

/* Seuil de facteur de charge au-delà duquel on déclenche un rehash */
#define LOAD_FACTOR_THRESHOLD 0.7

/*
 * hash — calcule l'index dans le tableau pour une clé donnée.
 * Utilise l'algorithme DJB2. Retourne directement un index valide
 * compris entre 0 et size-1 (modulo déjà appliqué).
 */
unsigned long hash(const char *key, int size);

/*
 * ht_check_resize — vérifie le facteur de charge actuel du tableau
 * et déclenche ht_resize() automatiquement si le seuil est dépassé.
 * À appeler par P3 après chaque insertion (SET).
 */
void ht_check_resize(HashTable *ht);

#endif /* HASH_H */