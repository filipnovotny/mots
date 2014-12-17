#define _CRT_SECURE_NO_DEPRECATE //pour VS2013, afin de pouvoir utiliser les fonctions classiques printf etc.
#define HIDE_REPEATED_WORDS

#include <stdio.h>
#include <stdlib.h>
#ifdef HIDE_REPEATED_WORDS
#include <string.h>
#endif
#include "defines.h"
#include "helpers.h"

/*
Complexité moyenne totale de l'algorithme: O(n) + O(nln(n)) + O(n) = O(n*ln(n))
*/

int main(int argc, char** argv){
	try{
		char* filename;
		if (argc > 1)
			filename = argv[1];
		else
		  filename = const_cast<char*>(DEFAULT_FILE);
		FILE *A = fopen(filename, "rb");
		if (!A)
			throw ERR_FILE_NOT_FOUND;

		reader_t reader(A);

		node_t *first = new node_t(), *last;		
		first->prev = NULL;

		node_t *cur = first;
		/*
		Lecture du fichier: O(n)
		Lit le fichier mot par mot et stocke les mots dans une liste doublement chainée.
		Les buffers sont alloués et les noeuds de meme. La mémoire est libérée plus tard
		*/
		for (char* word = reader.lire_mot(); word; word = reader.lire_mot()) {
			cur->word = word;
			cur->len = reader.len();
			cur->next = new node_t();
			cur->next->prev = cur;
			cur = cur->next;
		};

		fclose(A);
		last = cur;

		/*
		Lecture du fichier: O(nlnn) complexité moyenne
		La liste doublement chainée contient les mots dans le désordre.
		Nous effectuons un quicksort pour les ordonner suivant le double critère (tailles+alphabétique)
		*/
		quicksort(first, last->prev);

		/*
		Affichage des mots: O(n)
		L'énoncé n'est pas totallement clair sur la façon dont les mots doivent etre affichés.
		Si un mot est présent 2 fois, doit-il etre affiché 2 fois?
		La macro HIDE_REPEATED_WORDS permet d'afficher chaque mot distinct qu'une seule fois.
		Si l'on veut afficher le mot le nombre de fois qu'il apparait vraiment dans le texte, il
		suffit de désactiver la macro HIDE_REPEATED_WORDS.
		*/
#ifdef HIDE_REPEATED_WORDS
		char* word = NULL;
#endif
		size_t len = 0;
		int i = 0;
		for (cur = first; cur != last; cur = cur->next) {
			if (len != cur->len){
				printf("\nMots de %lu lettres\n", cur->len);
				len = cur->len;
			}
#ifdef HIDE_REPEATED_WORDS
			if (word == NULL || strcmp(word, cur->word)){
				printf("%d. %s\n", ++i, cur->word);
				word = cur->word;
			}			
#else			
			printf("%d. %s\n",++i,cur->word);
#endif			
		};

		node_t* cur_next;

		/*
		Libération espace: O(n)
		*/
		for (cur = first; cur; cur = cur_next) {
			cur_next = cur->next;
			delete[] cur->word;
			delete cur;
		}
		reader.print_alloc_report();
	}
	catch (int e){
		printf("CRITICAL ERROR: %s\n", errors[e]);
	}
	
	return 0;

}
