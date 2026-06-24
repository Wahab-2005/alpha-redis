# α-Redis

Moteur de stockage NoSQL In-Memory, inspiré de Redis — Projet de groupe (5 personnes).

**Date butoir :** Mercredi 05 Juillet 2026
**Rendu :** Document + codes sources + présentation orale

## Objectif

Reconstruire from scratch un moteur clé-valeur en mémoire, avec deux types de valeurs
(string et liste), en implémentant nous-mêmes les structures de données fondamentales
(table de hachage, gestion des collisions, liste doublement chaînée) pour obtenir des
accès en O(1).

## Architecture

```
alpha-redis/
│
├── main.c                  <- Point d'entrée, assemble tout (P5)
├── Makefile                <- Compile tout le projet en une commande
│
├── p1_tableau/
│   ├── tableau.h            <- Structure Entry + signatures (P1)
│   └── tableau.c            <- Implémentation du tableau dynamique (P1)
│
├── p2_hashage/
│   ├── hash.h                <- Signature de la fonction hash() (P2)
│   └── hash.c                <- Implémentation DJB2 (P2)
│
├── p3_collisions/
│   ├── liste_simple.h        <- Signatures SET / GET / DEL (P3)
│   └── liste_simple.c        <- Liste chaînée simple, gestion collisions (P3)
│
├── p4_liste/
│   ├── liste_double.h        <- Signatures LPUSH / RPUSH / LPOP / RPOP (P4)
│   └── liste_double.c        <- Liste doublement chaînée (P4)
│
└── p5_cli/
    ├── cli.h                 <- Signature de run_cli() (P5)
    └── cli.c                 <- Interpréteur de commandes + tests (P5)
```

## Règle d'or

Chaque personne ne modifie **que** son dossier. Pour utiliser une fonction d'un autre
module, on l'inclut via `#include "../p2_hashage/hash.h"` — on ne touche **jamais**
au `.c` de quelqu'un d'autre.

## Répartition des rôles

| Personne | Rôle |
|----------|------|
| P1 | Tableau dynamique & Coordination |
| P2 | Fonction de hachage (DJB2) |
| P3 | Liste chaînée simple — gestion des collisions, SET/GET/DEL |
| P4 | Liste doublement chaînée — LPUSH/RPUSH/LPOP/RPOP |
| P5 | Interpréteur de commandes (CLI) & Tests |

## Build

```bash
make
./alpha-redis
```

## Exemple d'utilisation

```
> SET nom Alice
OK
> GET nom
Alice
> LPUSH fruits pomme
OK (length: 1)
> RPOP fruits
pomme
> EXIT
Bye!
```

## Workflow Git

1. Chaque membre travaille sur sa propre branche : `git checkout -b p1-tableau` (etc.)
2. `tableau.h` (P1) doit être finalisé et pushé en premier — tous les autres modules en dépendent.
3. Pull request vers `main` une fois le module testé.
4. Revue de code par P1 avant le merge final.
