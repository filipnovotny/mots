#include <stdio.h>
#include <math.h>
#include <string.h>
#include "defines.h"
#include "helpers.h"

bool is_separator(const char c){
	int i = 0;
	
	for (char cur_separator = separators[i];
		i < SEPARATORS_COUNT && separators[i] != c;
		i++, cur_separator = separators[i]);

	return (i != SEPARATORS_COUNT);
}

char my_fgetc(FILE* A){
	char c = fgetc(A);
	if (ferror(A))
		throw ERR_READ_FILE;

	return c;
}

reader_t::reader_t(FILE *mon_fichier) :
										mon_fichier_(mon_fichier),
										chunk_size_(REASONABLE_CHUNK_SIZE),
										cursor_(buf_),
										word_count_(0),
										mean_(0.),
										M2_(0.),
										bytes_wasted_(0),
										bytes_allocated_(0),
										nb_reallocs_(0) {
	//obtient la taille du fichier
	fseek(mon_fichier_, 0, SEEK_END);
	file_size_ = ftell(mon_fichier_);
	fseek(mon_fichier_, 0, SEEK_SET);
}

void reader_t::update_stats(size_t word_size){
	word_count_++;
	double delta = (double)word_size - mean_;
	mean_ = mean_ + delta / (double)(word_count_);
	M2_ = M2_ + delta *((double)word_size - mean_); //moment d'ordre 2 pour le calcul de la variance

	bytes_wasted_ += (chunk_size_ - word_size);
	bytes_allocated_ += chunk_size_;	
}

size_t reader_t::alloc_size_heuristic() const{
	if (!mean_)
		return REASONABLE_CHUNK_SIZE; //si pas d'infos sur la moyenne, allouons une taille arbitraire 
	size_t remaining = file_size_ - (cursor_ - buf_);
	
	//renvoie la moyenne ou le nombre de caracteres restant du fichier si on est à la fin
	return MIN((size_t)round(mean_), chunk_size_ + remaining + 1);
}

size_t reader_t::realloc_size_heuristic() const{
	size_t new_size = 0;
	if (word_count_ < 4) //pas assez de données, contentons nous d'un allocateur exponentiel
		new_size = chunk_size_*2;
	else
		//redimentionnement intelligent, ajout d'un ecart type
		//Si par hasard, l'ecart type est de 0, il faut quand meme augmenter la taille, dans ce cas
		//augmentons de MIN_REALLOCATION_SIZE
		new_size = chunk_size_ + (size_t)(MAX(round(sqrt(M2_ / (word_count_ - 1))), MIN_REALLOCATION_SIZE));
	
	size_t remaining = file_size_ - (cursor_ - buf_);
	
	return MIN(new_size, chunk_size_+remaining+1/*+1 because of the last \0 character*/);
	
}


char *reader_t::lire_mot(){		
	char c; //Premier caractère du mot (s'il existe)
	//boucle passant tous les séparateurs jusqu'au premier mot
	for (c = (char)my_fgetc(mon_fichier_);
		!feof(mon_fichier_) && is_separator(c);
		c = (char)my_fgetc(mon_fichier_));

	if (feof(mon_fichier_)) return NULL;

	//alloc intelligente du buffer
	chunk_size_ = alloc_size_heuristic(); 
	cursor_ = buf_ = new char[chunk_size_];
	*cursor_ = c; //le curseur est sur le 1er caractère du mot, j'écris le 1er char
	
	//écriture des caractères jusqu'au prochain séparateur
	while (!feof(mon_fichier_) && !is_separator(*cursor_)) {
		cursor_++;
		if (cursor_ >= buf_ + chunk_size_){ //le mot est plus long que la mémoire allouée, redimentionnement
			size_t old_chunk_size = chunk_size_;
			char* buf_tmp = new char[old_chunk_size];
			memcpy(buf_tmp, buf_, old_chunk_size); //sauvegarde
			chunk_size_ = realloc_size_heuristic(); //redimentionnement intelligent

			delete buf_;
			size_t offset = cursor_ - buf_;
			buf_ = new char[chunk_size_]; //récupération des données
			cursor_ = buf_ + offset;
			memcpy(buf_, buf_tmp, old_chunk_size);
			delete buf_tmp;

			nb_reallocs_++;
		}

		*cursor_ = (char)my_fgetc(mon_fichier_);			
	} 

	if (!feof(mon_fichier_))
		update_stats(cursor_ - buf_);
	
	*cursor_ = '\0'; //terminons chaque mot par le caractere de fin
	return buf_;	
}

size_t reader_t::len() const{
	return cursor_ - buf_;
}

void reader_t::print_alloc_report() const{
	printf("bytes allocated: %d\nbytes wasted (unnecessary allocation) %d\n% % wasted: \
		   %lf\nnumber of reallocations:%d\n\n\n \
		   mean size of word:%fl\n",		   
		bytes_allocated_,
		bytes_wasted_,
		(double)bytes_wasted_ * 100. / (double)bytes_allocated_,
		nb_reallocs_,
		mean_);
}


char simplified(char _c) {
	//simplification des caractères accentués
	unsigned char c = (unsigned char)_c;
	c = c == 0xFD || c == 0xFF || c == 0xDD || c==0x9F ? 'y' : c;
	c = (c <= 0xFC && c >= 0xF9) || (c <= 0xDC && c >= 0xD9) ? 'u' : c;
	c = c == 0x9C || c == 0x8C || c == 0xF8 || c == 0xD8 || c == 0xD0 || (c <= 0xF6 && c >= 0xF2) || (c <= 0xD6 && c >= 0xD2) ? 'o' : c;
	c = c == 0xF1 || c == 0xD1 ? 'n' : c;
	c = (c <= 0xEF && c >= 0xEC) || (c <= 0xCF && c >= 0xCC) ? 'i' : c;
	c = (c <= 0xEB && c >= 0xE8) || (c <= 0xCB && c >= 0xC8) ? 'e' : c;
	c = c == 0xE7 || c == 0xC7 ? 'c' : c;
	c = c == 0x9E || c == 0x8E ? 'z' : c;
	c = c == 0x9A || c == 0x8A ? 's' : c;
	c = (c <= 0xE6 && c >= 0xE0) || (c <= 0xC6 && c >= 0xC0) ? 'a' : c;
	c = c == 0xDF ? 's' : c;
	if (c == 0xF6){
		int h = 0;

	}
	//tout en lowercase pour une bonne comparaison par la suite
	unsigned char c_lowercase = c>0x40 && c<0x5b ? c | 0x60 : c;
		
	return c_lowercase;
}

bool compare(node_t* node1, node_t* node2){	
	if (node1->len != node2->len) //tri par taille
		return node1->len < node2->len;
	else{ //ordre alphabétique
		for (unsigned int i = 0; i < node1->len; i++){
			if (node1->word[i] != node2->word[i])
				return simplified(node1->word[i]) < simplified(node2->word[i]);
		}
	}
	return true;
}

void swap(node_t* a, node_t* b){
	size_t tmp_len = a->len;
	char *tmp_word = a->word;

	a->len = b->len;
	b->len = tmp_len;
	
	a->word = b->word;
	b->word = tmp_word;
}

node_t* partition(node_t *low, node_t *high){
	node_t *i = low->prev;

	for (node_t *j = low; j != high; j = j->next)	
		if (compare(j, high)){
			i = (i == NULL) ? low : i->next;
			swap(i, j);
		}
	
	i = (i == NULL) ? low : i->next; 
	swap(i, high);
	return i;
}

void quicksort(node_t* low, node_t *high){
	if (high != NULL && low != high && low != high->next){
		node_t *p = partition(low, high);
		quicksort(low, p->prev);
		quicksort(p->next, high);
	}
}