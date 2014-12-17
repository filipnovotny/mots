#ifndef __DEFINES_H__
#define __DEFINES_H__

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define DEFAULT_FILE "test.txt" //Nom du fichier à lire
#define REASONABLE_CHUNK_SIZE 6		//C'est la taille du buffer par défaut avant que le système aie suffisemment d'information pour allouer intelligemment
#define SEPARATORS_COUNT 13			//Nombre de caractères listés comme séparateurs possibles dans le tableau separators
#define MIN_REALLOCATION_SIZE 1		//à chaque réalocation, la taille du buffer doit etre augmentée au minimum de MIN_REALLOCATION_SIZE

#include <stdlib.h> //pour size_t

extern const char separators[];			//liste de séparateurs

/////////////////////////////////////////////////////
////////////////////// ERRORS ///////////////////////
/////////////////////////////////////////////////////

#define ERR_FILE_NOT_FOUND	0
#define ERR_READ_FILE		1

extern const char* errors[];

#endif
