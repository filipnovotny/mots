#ifndef __HELPERS_H__
#define __HELPERS_H__

/*
Détermine si le caractère est un séparateur ou non (ex espace, retour à la ligne, etc.)
c : caractère à tester
retour: true si séparateur, false sinon
*/
bool is_separator(const char c);
/*
Lit un caractère dans un fichier.
A : ID fichier dans lequel lire
retour : la caractère lu
exceptions: ERR_READ_FILE si erreur
*/
char my_fgetc(FILE* A);

/*
Structure pour liste doublement chainée utilisée pour stocker les mots
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
	Variables liées à l'estimateur des moyenne et écart-type utilisé
	pour optimiser l'allocateur.
	*/
	size_t word_count_; //nombre de mots lus jusqu'alors
	double M2_;			//Moment d'ordre 2 utilisé pour calculer efficacement l'écart type partiel
	double mean_;		//moyenne partielle
	
	//Pour les statistiques liées à l'alloc
	size_t bytes_wasted_;	//octets alloués mais inutilisés
	size_t bytes_allocated_;//octets alloués (total)
	size_t nb_reallocs_;	//nombre de fois que l'on redimentionne le buffer_ (ex: buffer trop court pour mot trop long)
public:
	/*
	Constructeur. Initialise les variables privées y compris celles des stats
	mon_fichier : fichier qui est lu
	*/
	reader_t(FILE *mon_fichier);
	
	/*
	Alloue un buffer et lit un mot dans ce buffer.
	Comme il s'agit d'un nouveau mot, il est impossible de connaitre sa longueur à l'avance. 
	Il faut allouer "à l'aveugle".
	Cette méthode effectue une allocation intelligente de la mémoire de façon à minimiser le nombre de
	réalocations tout en économisant la mémoire (inutile d'allouer trop pour un petit mot).
	
	Au fur et à mesure que les mots sont lus, on tient en compte de la distribution des lettres/mot.
	La variance et la moyenne partielle de cette distribution est calculée à chaque itération, algo en O(1)
	Lors de la lecture d'un nouveau mot, la longueur du nouveau mot est prédite par la moyenne des longueurs
	des mots précédents. Si le mot s'avère plus long, la réalocation s'effectue par ajout d'écats-types.

	Empiriquement, la méthode économise 10% d'espace mémoire sur un texte de 2651 mots en langue anglaise par rapport
	à un allocateur à croissance exponentielle.

	retour : mot actuel dans le buffer buf_ terminé par le caractère nul
	*/
	char *lire_mot();

	/*
	Met à jour les statistiques internes (calcul des moments pour les estimateurs)
	word_size : taille du mot actuel
	*/
	void update_stats(size_t word_size);
	/*
	Prédit la taille du nouveau mot en s'appuyant sur la longueur des mots précedemment lus.
	Le but de cette fonction est de ne pas allouer trop de mémoire mais d'éviter la réalocation.
	L'estimation est la moyenne partielle. Algo en O(1).
	L'idéal aurait été d'avoir un estimateur de la médiane mais je ne pense pas trouver celui-ci en O(1).
	Je sacrifie donc un peu de mémoire au profit de la vitesse.

	retour : espace à allouer
	*/
	size_t alloc_size_heuristic() const;
	/*
	Réaloue la mémoire lorsque le mot lu est plus long que le buffer alloué.
	Le buffer est augmenté consécutivement par les écarts-types constatés.
	Exemple si on a alloué un buffer de taille M, la nouvelle taille sera M+sigma

	retour : taille du nouveau buffer
	*/
	size_t realloc_size_heuristic() const;

	/*
	renvoie la taille du mot lu	
	*/
	size_t len() const;

	/*
	Affiche les stats comme la longueur moyenne du mot, nb de séparateurs, etc
	*/
	void print_alloc_report() const;
};

/*
	Fonction de comparaison.
	La consigne "trier par longueur de mot et puis par ordre alphabétique"
	n'implique en réalité qu'une seule relation d'ordre, on peut donc l'incorporer entièrement dans une fonction
	de comparaison.

	node1,node2 : noeuds à comparer. Ils contiennent les mots à trier.

	retour : true si le noeud comporte un mot plus court, false si plus long
			 dans le cas ou les 2 longueures sont egales, true si le mot est alphabétiquement plus petit
			 false si plus grand
*/
bool compare(node_t*  node1, node_t* node2);
/*
	échange les données contenues dans les noeuds a et b
*/
void swap(node_t* a, node_t* b);
/*
	fonction de partitionnement du quicksort adaptées aux listes doublement chainées

	retour: noeud du pivot
*/
node_t* partition(node_t *low, node_t *high);
/*
	quicksort adapté aux listes doublement chainées.	
*/
void quicksort(node_t* low, node_t *high);

#endif