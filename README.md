mots
====

Le programme prend un fichier en entrée sans supposer la longueur maximale de ce fichier ni de mots.
Il "apprend" la distribution des mots au fur et à mesure que le fichier est lu ce qui permet une allocation optimale de la mémoire.
Le programme n'utilise pas la STL mais utilise certaines fonctions primitives des bibliotheques classiques C comme strcmp, memcpy dont le re-développement a peu d'intéret.

!!NB!!: Il ne m'a pas paru clair de l'énoncé s'il faut lister les mots de façon distincte ou répétée lorsqu'il le sont.
Par exemple, le fichier "test test" doit il lister 1. test 2. test ou juste 1. test?
Une macro a étée définie: HIDE_REPEATED_WORDS. Si activée (par défaut), un mot répété ne va etre listé 1 fois (tout en restant sensible à la case)
Si elle est désactivée, le mot sera listé autant de fois qu'il apparait.

Plateformes:
===========
Testé sous Windows VS2013 ainsi que linux ubuntu 14+valgrind.
Aucune fuite de mémoire détectée lors des tests.

Utilisation:
===========

./mots fichier.txt

Sous windows/VC++, il suffit de charger les fichiers dans un projet vide et le lancer avec comme argument le fichier à analyser
Sous linux:
make
./mots fichier.txt

Tri des mots:
=============

Pour le tri des mots, l'algo de tri rapide a été choisi au détriment par exemple d'une table de hachage qu'il faudrait trier partiellement de toute manière.
Le tri rapide implémenté sur une liste doublement chainée permet une économie de mémoire (on ne stocke que ce dont on a besoin) et réalise le tri par ordre
alphabétique et par longueur de mot en 1 fois.

Lecture du fichier:
===================

La méthode lire_mot de la classe reader_t lit le fichier mot par mot. Comme la taille des mots à l'avance est inconnue, il faut estimer une allocation intelligente.
Au fur et à mesure que les mots sont lus, on tient en compte de la distribution des lettres/mot.

La variance et la moyenne partielle de cette distribution est calculée à chaque itération, algo en O(1)
Lors de la lecture d'un nouveau mot, la longueur du nouveau mot est prédite par la moyenne des longueurs
des mots précédents. Si le mot s'avère plus long, la réalocation s'effectue par ajout d'écats-types.

Empiriquement, la méthode économise 10% d'espace mémoire sur un texte de 2651 mots en langue anglaise par rapport
à un allocateur à croissance exponentielle.

Conventions de programmation:
============================

les commentaires sont en Français, les specs des fonctions sont dans les headers. Les noms de variables+fonctions sont en anglais hormi celles fixées par l'exercice: ex: lire_mot. 
Il s'est avéré que l'utilisation en tant que méthode de lire_mot est plus judicieuse que en tant que fonction, j'ai donc pris la liberté de changer son prototype de char* lireMot(FILE*) en char* lire_mot, le fichier étant global à la classe.

les noms de types et de variables sont en minuscules, les variables privées sont affixées de var_, les types par type_t.


Complexité:
===========

L'accent a été mis sur l'économie de la mémoire tout en minimisant la complexité autant que possible étant donné la 1ere contrainte.
avec n le nombre de mots dans le fichier, complexité moyenne
Lecture: O(n)
Tri: O(n*log(n))
Affichage: O(n)
Libération mémoire: O(n)
Total: O(n*log(n))
