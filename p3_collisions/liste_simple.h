#ifndef LISTE_SIMPLE_H
#define LISTE_SIMPLE_H

/*
 * liste_simple.h — Personne 3
 *
 * Gestion des collisions par chainage separe + commandes SET / GET / DEL.
 * Chaque case du tableau (bucket) pointe vers une liste chainee simple
 * d'entrees ayant le meme index de hachage.
 *
 * Depend de :
 *   - p1_tableau/tableau.h  (structures Entry et HashTable)
 *   - p2_hashage/hash.h     (fonction hash)
 */

#include "../p1_tableau/tableau.h"
#include "../p2_hashage/hash.h"

/*
 * db_set — Complexite O(1) amortie
 *
 * Calcule le hash de la cle, parcourt la liste chainee du bucket
 * correspondant. Si la cle existe deja, met a jour sa valeur.
 * Sinon, insere un nouveau maillon en tete de la chaine.
 * Declenche un redimensionnement si count/size > 0.7
 */
void  db_set(HashTable *ht, const char *key, const char *value);

/*
 * db_get — Complexite O(1)
 *
 * Calcule le hash de la cle, parcourt la liste chainee du bucket.
 * Retourne la valeur associee, ou NULL si la cle n'existe pas.
 */
char* db_get(HashTable *ht, const char *key);

/*
 * db_del — Complexite O(1)
 *
 * Calcule le hash de la cle, trouve le maillon dans la chaine,
 * le retire proprement (relie le precedent au suivant) et libere la memoire.
 * Ne fait rien si la cle n'existe pas.
 */
void  db_del(HashTable *ht, const char *key);

/*
 * db_set_list — Complexite O(1)
 *
 * Variante de db_set pour stocker une liste doublement chainee (type LIST).
 * Utilise le champ list_value de Entry au lieu de str_value.
 * Appelee par P5 lors des commandes LPUSH / RPUSH.
 */
void  db_set_list(HashTable *ht, const char *key, void *list);

/*
 * db_get_entry — Complexite O(1)
 *
 * Retourne le pointeur vers l'Entry complete (pas juste la valeur string).
 * Utile pour P5 quand il doit verifier le type (STRING ou LIST)
 * avant d'executer une commande.
 */
Entry* db_get_entry(HashTable *ht, const char *key);

#endif /* LISTE_SIMPLE_H */
