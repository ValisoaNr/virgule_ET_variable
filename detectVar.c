#include<stdio.h>
#include<stdbool.h>

struct Variable
{
    char *nom_fonc;
    char *varType;
    char *nom_var;
};
typedef struct Variable Variable;

void variable(char *nomFichier);
bool declaration(char *ligne_c);
void prendNom_fonc(char *ligne, char *emplacement);
void formateDeclaration(char *fichier_c, char *ficVar);
bool compareVar(Variable var1, Variable var2);
bool verifieVar(Variable var, char *nomFichier);
void CoupeUnMot(char *ligne, char *emplacement);
int insertMotcle(char **motCle);
bool testerVar(char *mot);
int preleveVar(char *ligne, Variable *tabVarLigne);
