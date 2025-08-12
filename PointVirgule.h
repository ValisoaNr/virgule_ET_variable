#include<stdbool.h>

void testVir(char *fichier);
int trouveErreur(char *ligne, bool instruction);
int listerInst#include<stdbool.h>

struct Erreur
{
    int tache;
    int num_ligne;
};
typedef struct Erreur Erreur;

void testVir(char *fichier);
int trouveErreur(char *ligne, bool instruction);
int listerInst(char *fichier, int *nLigne);
int trouveFin(char *ligne);
void supprimeFin(char *fichier, int num_ligne);
void insererFin(char *fichier, int num_ligne, char caractere);
void supprimePV(char *fichier, int num_ligne);
void corrige(char *fichier, Erreur defaut);(char *fichier, int *nLigne);
