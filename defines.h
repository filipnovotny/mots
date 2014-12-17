#ifndef __DEFINES_H__
#define __DEFINES_H__

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define DEFAULT_FILE "test.txt" //Nom du fichier � lire
#define REASONABLE_CHUNK_SIZE 6		//C'est la taille du buffer par d�faut avant que le syst�me aie suffisemment d'information pour allouer intelligemment
#define SEPARATORS_COUNT 13			//Nombre de caract�res list�s comme s�parateurs possibles dans le tableau separators
#define MIN_REALLOCATION_SIZE 1		//� chaque r�alocation, la taille du buffer doit etre augment�e au minimum de MIN_REALLOCATION_SIZE

#include <stdlib.h> //pour size_t

extern const char separators[];			//liste de s�parateurs

/////////////////////////////////////////////////////
////////////////////// ERRORS ///////////////////////
/////////////////////////////////////////////////////

#define ERR_FILE_NOT_FOUND	0
#define ERR_READ_FILE		1

extern const char* errors[];

#endif
