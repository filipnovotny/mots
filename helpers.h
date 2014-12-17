#ifndef __HELPERS_H__
#define __HELPERS_H__

/*
D�termine si le caract�re est un s�parateur ou non (ex espace, retour � la ligne, etc.)
c : caract�re � tester
retour: true si s�parateur, false sinon
*/
bool is_separator(const char c);
/*
Lit un caract�re dans un fichier.
A : ID fichier dans lequel lire
retour : la caract�re lu
exceptions: ERR_READ_FILE si erreur
*/
char my_fgetc(FILE* A);

/*
Structure pour liste doublement chain�e utilis�e pour stocker les mots
*/
struct node_t{
	char* word; //pointeur vers le mot
	size_t len; //longueur du mot
	//chainage
	struct node_t *next; 
	struct node_t *prev;
};

/*
Classe responsable de la lecture et allocations des mots dans le fichier
*/
class reader_t{
private:
	char* buf_; //Buffer contenant le mot actuel lu

	FILE *mon_fichier_; //ID fichier lu
	size_t chunk_size_; //taille du buffer correspondant au mot actuel
	char* cursor_; //position actuelle dans le buffer actuel buf_
	size_t file_size_; //taille du fichier lu

	/*
	Variables li�es � l'estimateur des moyenne et �cart-type utilis�
	pour optimiser l'allocateur.
	*/
	size_t word_count_; //nombre de mots lus jusqu'alors
	double M2_;			//Moment d'ordre 2 utilis� pour calculer efficacement l'�cart type partiel
	double mean_;		//moyenne partielle
	
	//Pour les statistiques li�es � l'alloc
	size_t bytes_wasted_;	//octets allou�s mais inutilis�s
	size_t bytes_allocated_;//octets allou�s (total)
	size_t nb_reallocs_;	//nombre de fois que l'on redimentionne le buffer_ (ex: buffer trop court pour mot trop long)
public:
	/*
	Constructeur. Initialise les variables priv�es y compris celles des stats
	mon_fichier : fichier qui est lu
	*/
	reader_t(FILE *mon_fichier);
	
	/*
	Alloue un buffer et lit un mot dans ce buffer.
	Comme il s'agit d'un nouveau mot, il est impossible de connaitre sa longueur � l'avance. 
	Il faut allouer "� l'aveugle".
	Cette m�thode effectue une allocation intelligente de la m�moire de fa�on � minimiser le nombre de
	r�alocations tout en �conomisant la m�moire (inutile d'allouer trop pour un petit mot).
	
	Au fur et � mesure que les mots sont lus, on tient en compte de la distribution des lettres/mot.
	La variance et la moyenne partielle de cette distribution est calcul�e � chaque it�ration, algo en O(1)
	Lors de la lecture d'un nouveau mot, la longueur du nouveau mot est pr�dite par la moyenne des longueurs
	des mots pr�c�dents. Si le mot s'av�re plus long, la r�alocation s'effectue par ajout d'�cats-types.

	Empiriquement, la m�thode �conomise 10% d'espace m�moire sur un texte de 2651 mots en langue anglaise par rapport
	� un allocateur � croissance exponentielle.

	retour : mot actuel dans le buffer buf_ termin� par le caract�re nul
	*/
	char *lire_mot();

	/*
	Met � jour les statistiques internes (calcul des moments pour les estimateurs)
	word_size : taille du mot actuel
	*/
	void update_stats(size_t word_size);
	/*
	Pr�dit la taille du nouveau mot en s'appuyant sur la longueur des mots pr�cedemment lus.
	Le but de cette fonction est de ne pas allouer trop de m�moire mais d'�viter la r�alocation.
	L'estimation est la moyenne partielle. Algo en O(1).
	L'id�al aurait �t� d'avoir un estimateur de la m�diane mais je ne pense pas trouver celui-ci en O(1).
	Je sacrifie donc un peu de m�moire au profit de la vitesse.

	retour : espace � allouer
	*/
	size_t alloc_size_heuristic() const;
	/*
	R�aloue la m�moire lorsque le mot lu est plus long que le buffer allou�.
	Le buffer est augment� cons�cutivement par les �carts-types constat�s.
	Exemple si on a allou� un buffer de taille M, la nouvelle taille sera M+sigma

	retour : taille du nouveau buffer
	*/
	size_t realloc_size_heuristic() const;

	/*
	renvoie la taille du mot lu	
	*/
	size_t len() const;

	/*
	Affiche les stats comme la longueur moyenne du mot, nb de s�parateurs, etc
	*/
	void print_alloc_report() const;
};

/*
	Fonction de comparaison.
	La consigne "trier par longueur de mot et puis par ordre alphab�tique"
	n'implique en r�alit� qu'une seule relation d'ordre, on peut donc l'incorporer enti�rement dans une fonction
	de comparaison.

	node1,node2 : noeuds � comparer. Ils contiennent les mots � trier.

	retour : true si le noeud comporte un mot plus court, false si plus long
			 dans le cas ou les 2 longueures sont egales, true si le mot est alphab�tiquement plus petit
			 false si plus grand
*/
bool compare(node_t*  node1, node_t* node2);
/*
	�change les donn�es contenues dans les noeuds a et b
*/
void swap(node_t* a, node_t* b);
/*
	fonction de partitionnement du quicksort adapt�es aux listes doublement chain�es

	retour: noeud du pivot
*/
node_t* partition(node_t *low, node_t *high);
/*
	quicksort adapt� aux listes doublement chain�es.	
*/
void quicksort(node_t* low, node_t *high);

#endif