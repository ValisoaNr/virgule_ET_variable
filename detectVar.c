#include<stdlib.h>
#include<string.h>
#include"action.h"
#include"detectVar.h"

#define TAILLE_LIGNE 1000
#define MOT 50
#define NB_VARMAX 20

void variable(char *nomFichier)
{
    /*
        Enregistrer les variables declares et le fonction ou il se trouve
        Puis verifier chaque variable utillisee
    */
    FILE *ptrFic;
    int i, longLigne, nbVarLigne, num_ligne, saut;
    char *ligne, *tmp, *fonc_actuel, *comm, *mot;
    Variable *varLigne;
    bool Edecl, motCle;

    //inserer les variables declarees dans le fichier "fichierVar.txt"
    formateDeclaration(nomFichier, "fichierVar.txt");

    ptrFic = fopen(nomFichier, "r");
    if(ptrFic == NULL)
    {
        printf("Une erreur s'est passé lors de l'ouverture\n");
        exit (1);
    }
    //si l'ouverture du fichier est valide passer au allocation des besoins
    ligne = malloc(TAILLE_LIGNE*sizeof(char));
    tmp = malloc(TAILLE_LIGNE*sizeof(char));
    mot = malloc(MOT*sizeof(char));
    varLigne = malloc(NB_VARMAX*sizeof(Variable));
    fonc_actuel = malloc(MOT*sizeof(char));
    for(i=0; i<NB_VARMAX; i++)
    {
        varLigne[i].nom_fonc = malloc( MOT * sizeof(char) );
        varLigne[i].nom_var = malloc( MOT * sizeof(char) );
        varLigne[i].varType = malloc( MOT * sizeof(char) );
    }

    num_ligne = 1;
    //feof(ptrFic) retournera 1 en fin de fichier
    while(!(feof(ptrFic)))
    {
        //mettre une ettiquette pour revenir facilement avec "goto "
        prendreLigne :

        fgets(ligne, TAILLE_LIGNE, ptrFic);
        
        //copier la ligne pour l'etudier puis l'afficher
        strcpy(tmp, ligne);

        //enlever les parties de la ligne qui ne doivent pas etre analyser
        saut = repareLigne(tmp, ptrFic);
        longLigne = strlen (tmp);
        i=0;
        while((tmp[i]==' ') || (tmp[i]=='\t'))
        {
            i++;
        }
        if((tmp[i]=='#') || (tmp[i]=='\n'))
        {
            num_ligne++;
            goto prendreLigne;
        }
        //voyons si la ligne possede une declaration
        Edecl=declaration(tmp);

        //si defintion de fonction enregistrer le nom comme fonction actuel
        comm=strchr(tmp, '(');
        if((comm!=NULL) && (Edecl))
        {
            comm=strchr(tmp, ')');
            if(comm!=NULL)
            {
                prendNom_fonc(tmp, mot);
                //teste si ce n'est pas un mot cle en c
                motCle = testerVar(mot);
                if(!(motCle))
                {
                    strcpy(fonc_actuel, mot);
                    num_ligne++ ;
                    goto prendreLigne;
                }
            }
        }
        //si ligne de declaration sauter
        if(Edecl)
        {
            num_ligne++;
            goto prendreLigne;
        }
        //prelevement des variables  de la ligne vers varLigne
        nbVarLigne = preleveVar(tmp, varLigne);
        
        for(i=0; i<nbVarLigne; i++)
        {
            strcpy(varLigne[i].nom_fonc, fonc_actuel);
            Edecl = false;
            Edecl = verifieVar(varLigne[i], "fichierVar.txt");
            
            if(!(Edecl))
            {
                printf("ligne n°%d : %s \n",num_ligne , ligne);
                printf("dans %s\n",fonc_actuel);
                printf("--> '%s' non declare\n\n",varLigne[i].nom_var);
            }
        }
        
        num_ligne = num_ligne + saut;
        num_ligne++;
    }

    free(varLigne);
    free(ligne);
    free(mot);
    free(fonc_actuel);
    free(tmp);
    fclose(ptrFic);
}
bool declaration(char *ligne_c)
{
    // TESTER SI UNE PHRASE EST UNE DECLARATION EN C

    bool Edecl;
    char *comm;
    int i, j, nbType;
    char **type;

    nbType=7;
    type=malloc(nbType*sizeof(char *));
    for(i=0; i<nbType; i++)
    {
        type[i]=malloc((MOT+1)*sizeof(char));
    }

    // definition des types de variable
        i=0;
        strcpy(type[i], "int");
        i++;
        strcpy(type[i], "char");
        i++;
        strcpy(type[i], "double");
        i++;
        strcpy(type[i], "float");
        i++;
        strcpy(type[i], "bool");
        i++;
        strcpy(type[i], "unsigned");
        i++;
        strcpy(type[i], "FILE");
    //
    Edecl=false;
    for(i=0; i<nbType; i++)
    {
        // chercher d'abord "type + * " puis "type + espace"
        strcat(type[i], "*");
        comm=NULL;
        comm=strstr(ligne_c, type[i]);

        if(comm==NULL)
        {
            j=strlen(type[i]);
            type[i][(j-1)]=' ';
            strcat(type[i], "*");
            comm=strstr(ligne_c, type[i]);
        }
        if(comm==NULL)
        {
            j=strlen(type[i]);
            type[i][(j-2)]='\0';
            strcat(type[i], "\t");
            comm=strstr(ligne_c, type[i]);
        }
        if(comm==NULL)
        {
            j=strlen(type[i]);
            type[i][(j-1)]=' ';
            comm=strstr(ligne_c, type[i]);
        }
        if(comm!=NULL)
        {
            Edecl=true;
            break;
        }
    }
    free(type);

    return Edecl;
}
void formateDeclaration(char *fichier_c, char *ficVar)
{
    //detecter les declarations de variables dans fichier_c et enregistrer dans ficVar
    //FORMAT ficVar : fonction   types   nomVar
    /* " TYPE* Dim2 " ; " TYPE** Dim3 " */

    FILE *pf_c, *pf_ficVar;
    int longLigne, i, j, debut, fin, nbVar;
    char *ligne_c, *comm, *mot;
    char tmp;
    bool Edecl, virgule;
    char **varLigne;
    Variable var;

    pf_c=fopen(fichier_c, "r");
    if(pf_c==NULL)
    {
        printf("erreur d'ouverture de fichier_c\n");
        exit(-1);
    }

    // 'w+' pour pouvoir ecrire et creer en cas d'inexistence
    pf_ficVar=fopen(ficVar, "w+");
    if(pf_ficVar==NULL)
    {
        printf("erreur de fichier de reception !\n");
        exit (-2);
    }

    //faire les allocations si l'ouverture s'est passee
    ligne_c = malloc(TAILLE_LIGNE*sizeof(char));
    varLigne = malloc(NB_VARMAX*sizeof(char *));
    mot = malloc(NB_VARMAX*sizeof(char));
    for(i=0; i<NB_VARMAX; i++)
    {
        varLigne[i] = malloc(MOT*sizeof(char));
    }
    var.nom_fonc=malloc(MOT*sizeof(char));
    var.varType=malloc(MOT*sizeof(char));
    var.nom_var=malloc(MOT*sizeof(char));
    //feof(ptrFic) retournera 1 en fin de fichier
    while(!(feof(pf_c)))
    {
        //mettre une ettiquette pour revenir facilement avec "goto "
        prendreLigne :
        fgets(ligne_c, TAILLE_LIGNE, pf_c);

        // enlever les guillements puis les commentaires
        repareLigne(ligne_c, pf_c);
        longLigne = strlen(ligne_c);

        i=0;
        while((ligne_c[i]==' ') || (ligne_c[i]=='\t'))
        {
            i++;
        }
        if((ligne_c[i]=='#') || (ligne_c[i]=='\n'))
        {
            goto prendreLigne;
        }
        //voyons si la ligne possede une declaration
        Edecl=declaration(ligne_c);
        if( (!(Edecl)) && (!(feof(pf_c))) )
        {
            goto prendreLigne;
        }
        //si fonction enregistrer le nom comme fonction actuel
        comm=strchr(ligne_c, '(');
        if(comm!=NULL)
        {
            comm=strchr(ligne_c, ')');
            if(comm!=NULL)
            {
                prendNom_fonc(ligne_c, mot);
                //teste si ce n'est pas un mot cle en c
                Edecl = testerVar(mot);
                if(!(Edecl))
                {
                    strcpy(var.nom_fonc, mot);
                }
            }
        }
    
        //Extraire type && nom VARIABLE
        longLigne=strlen(ligne_c);
        Edecl=declaration(ligne_c);
        while(Edecl)
        {
            //enlevement du type de variable
            CoupeUnMot(ligne_c, var.varType);

            while((ligne_c[i]==' ') || (ligne_c[i]=='\t'))
            {
                i++;
            }
            nbVar=0;
            j=0;
            virgule=false;
            //prelevement des variables de la ligne de declaration
            while((ligne_c[i]!=';') && (ligne_c[i]!='\n'))
            {
                // declaration de plusieurs variables
                if(ligne_c[i]==',')
                {
                    virgule=true;
                    ligne_c[i]=' ';
                    varLigne[nbVar][j]='\0';
                    Edecl=declaration(ligne_c);
                    if(Edecl)
                    {
                        break;
                    }
                    nbVar++;
                    i=0;
                    while((ligne_c[i]==' ') || (ligne_c[i]=='\t'))
                    {
                        i++;
                    }
                    j=0;
                }
                else if(ligne_c[i]=='=')
                {
                    //declaration avec initialisation
                    ligne_c[i]=' ';
                    i++;
                    while((ligne_c[i]!=',') && (ligne_c[i]!=';'))
                    {
                        //cas de tableau initialise en declaration
                        if(ligne_c[i]=='{')
                        {
                            do
                            {
                                ligne_c[i]=' ';
                                i++;
                            }
                            while(ligne_c[i]!='}');
                            ligne_c[i]=' ';
                        }
                        ligne_c[i]=' ';
                        i++;
                    }
                }
                else
                {
                    //declaration d'une tableau
                    if(ligne_c[i]=='[')
                    {
                        do
                        {
                            ligne_c[i]=' ';
                            i++;
                        }
                        while(ligne_c[i]!=']');
                        ligne_c[i]=' ';
                        if(!(virgule))
                        {
                            //declaration tableau enregitrer le nom en type *
                            strcat(var.varType, "*");
                        }
                    }
                    else if(ligne_c[i]=='*')
                    {
                        ligne_c[i]=' ';
                        if(!(virgule))
                        {
                            strcat(var.varType, "*");
                        }
                        i++;
                    }
                    else
                    {
                        varLigne[nbVar][j]=ligne_c[i];
                        ligne_c[i]=' ';
                        j++;
                        i++;
                    }
                }
                //revisionner la ligne depuis la tete car chaque mot enregistrer est vider
                i=0;
                while((ligne_c[i]==' ') || (ligne_c[i]=='\t'))
                {
                    i++;
                }
            }
            varLigne[nbVar][j]='\0';
            //enregistrement de chaque variable declare dans le fichier
            for(i=0; i<=nbVar; i++)
            {
                strcpy(var.nom_var, varLigne[i]);
                fprintf(pf_ficVar, "%s\t\t%s\t\t%s\n", var.nom_fonc, var.varType, var.nom_var);
            }
            var.varType[0]='\0';
            // retester si encore declaration
            Edecl=declaration(ligne_c);
        }
    }
    
    free(ligne_c);
    free(varLigne);
    free(mot);
    free(var.nom_fonc);
    free(var.varType);
    free(var.nom_var);

    fclose(pf_c);
    fclose(pf_ficVar);
}
bool verifieVar(Variable var, char *nomFichier)
{
    /*
        verifier l'existence de var dans nomFichier
    */
    bool resultat;
    Variable tmpVar;
    FILE *fic_var;

    fic_var=fopen(nomFichier, "r");
    if(fic_var==NULL)
    {
        printf("fichier de variable inexistant\n");
        return (2);
    }
    tmpVar.nom_fonc = malloc(MOT*sizeof(char));
    tmpVar.varType = malloc(MOT*sizeof(char));
    tmpVar.nom_var = malloc(MOT*sizeof(char));

    resultat=false;
    while(!(feof(fic_var)))
    {
        //prendre chaque ligne selon le formatage par "void formateDeclaration(char *fichier_c, char *ficVar)"
        fscanf(fic_var, "%s\t\t%s\t\t%s\n", tmpVar.nom_fonc, tmpVar.varType, tmpVar.nom_var);

        //comparer chaque variable declarer avec var puis sortir s'il existe
        resultat=compareVar(tmpVar, var);
        if(resultat)
        {
            break;
        }
    }
    
    free(tmpVar.nom_fonc);
    free(tmpVar.varType);
    free(tmpVar.nom_var);
    fclose(fic_var);

    return (resultat);
}
bool compareVar(Variable var1, Variable var2)
{
    /*
        retourner true si var1 et var2 sont identiques
    */
    bool identique;
    int nomFonc, nom, type;

    identique=false;
    nomFonc=strcmp(var1.nom_fonc, var2.nom_fonc);
    nom=strcmp(var1.nom_var, var2.nom_var);

    if(((nomFonc)==0) && ((nom)==0))
    {
        identique=true;
    }

    return (identique);
}
void prendNom_fonc(char *ligne, char *emplacement)
{
    /*
        COUPER le nom de la fonction dans ligne vers emplacement
        en enlevant aussi les parentheses
    */
    int i, j, longLigne, debut, fin;
    char tmp;

    longLigne=strlen(ligne);
    for(i=0; i<longLigne; i++)
    {
        if(ligne[i] == ')')
        {
            ligne[i] = ' ';
            break;
        }
    }
    for(i=0; i<longLigne; i++)
    {
        if(ligne[i] == '(' )
        {
            ligne[i] = ' ';
            i-- ;
            j = 0;
            while((ligne[i]==' ') || (ligne[i]=='\t'))
            {
                i--;
            }
            //enregistrer le nom depuis "(" [ INVERSEMENT ]
            while((ligne[i] != ' ') && (ligne[i] != '\t') && (i>0))
            {
                emplacement[j] = ligne[i];
                ligne[i]=' ';
                i--;
                j++;
            }
            //enlever le valeur de retour du fonction ( inutile pour nous )
            while(i>=0)
            {
                ligne[i]=' ';
                i--;
            }
            //rectifier l'enregistrement a l'envers
            emplacement[j] = '\0';
            j=strlen(emplacement);
            debut = 0;
            fin = j-1;
            while(debut < fin)
            {
                tmp = emplacement[debut];
                emplacement[debut] = emplacement[fin];
                emplacement[fin] = tmp;
                debut++;
                fin--;
            }
            break;
        }
    }
}
int preleveVar(char *ligne, Variable *tabVarLigne)
{
    /*
        enregistrer les varibles dans lignes vers "Variable *tabVarLigne"
    */
    int nbVar, longLigne, i, j;
    bool nom_var;
    char *mot;

    mot=malloc(MOT*sizeof(char));
    nbVar=0;
    //parcourrir ligne jusqu'a sa fin
    longLigne=strlen(ligne);
    i=0;
    while(( i !=  longLigne))
    {
        //prendre mot par mot
        CoupeUnMot(ligne, mot);
        
        //tester si le mot prise n'est pas une mot cle en c
        nom_var=testerVar(mot);
        //cette fonction renvoie true si c'est un mot cle en donc on doit l'inverser
        nom_var=!(nom_var);
        
        //tester si ce n'est pas une nom de fonction
        i=0;
        while(((ligne[i]==' ') || (ligne[i]=='\t')) && ( i !=  longLigne) )
        {
            i++;
        }
        if(ligne[i]=='(')
        {
            ligne[i]==' ';
            nom_var=false;
        }
        // ne pas prendre mot vide
        if(mot[0]=='\0')
        {
            nom_var=false;
        }
        if((nom_var))
        {
            strcpy(tabVarLigne[nbVar].nom_var, mot);
            mot[0]='\0';
            nbVar++;
        }
        i=0;
        while(((ligne[i]==' ') || (ligne[i]=='\t')) && ( i !=  longLigne) )
        {
            i++;
        }
    }
    
    free(mot);

    return (nbVar);
}
bool testerVar(char *mot)
{
    /*
        retourne true si mot est un mot cle en langage c
    */
    bool resultat;
    int i, nbMotCle, test;
    char **motCle;

    // Quelque mot cle en c vers char **motCle
    nbMotCle = MOT;
    motCle = malloc(nbMotCle*sizeof(char*));
    for(i=0; i<nbMotCle; i++)
    {
        motCle[i] = malloc(MOT*sizeof(char));
    }
    //prendre les mots cles vers 
    nbMotCle = insertMotcle( motCle );
    resultat=false;
    //comparer le mot avec chaque element de mot cle
    for(i=0; i<nbMotCle; i++)
    {
        test=strcmp(motCle[i], mot);
        if(test==0)
        {
            resultat=true;
            break;
        }
    }
    
    return (resultat);
}
void CoupeUnMot(char *ligne, char *emplacement)
{
    /*
        deplace le premier mot de ligne vers emplacement
    */
    int i, j, longLigne, nbSigFin, position;
    char chiffre[11];
    bool symbole;
    char sigFin[32];

    //definition des caracteres qui ne doit pas etre prise
    sigFin[0]='"';
    sigFin[1]='(';
    sigFin[2]=' ';
    sigFin[3]=')';
    sigFin[4]='{';
    sigFin[5]='}';
    sigFin[6]='[';
    sigFin[7]=']';
    sigFin[8]='-';
    sigFin[9]='+';
    sigFin[10]='%';
    sigFin[11]='&';
    sigFin[12]='*';
    sigFin[13]='/';
    sigFin[14]=';';
    sigFin[15]=',';
    sigFin[16]='.';
    sigFin[17]='{';
    sigFin[18]='}';
    sigFin[19]='|';
    sigFin[20]='!';
    sigFin[21]='=';
    sigFin[22]=':';
    sigFin[23]='?';
    sigFin[24]='<';
    sigFin[25]='#';
    sigFin[26]='>';
    sigFin[27]='%';
    sigFin[28]='\t';
    sigFin[29]='\n';
    sigFin[30]='\\';
    sigFin[31]='\'';
    //les chiffres comme chaine de caractere pour le tester avec le chaine de caractere
    chiffre[0]='1';
    chiffre[1]='2';
    chiffre[2]='3';
    chiffre[3]='4';
    chiffre[4]='5';
    chiffre[5]='6';
    chiffre[6]='7';
    chiffre[7]='8';
    chiffre[8]='9';
    chiffre[9]='0';
    
    nbSigFin=strlen(sigFin);
    //saut des espaces et tabulation
    position=0;
    while((ligne[position]==' ') || (ligne[position]=='\t'))
    {
        position++;
    }
    //deplacer de caractere en caractere en vidant de ligne chaque caractere pris
    longLigne=strlen(ligne);
    symbole=false;
    //commence par un symbole ou par une chaine de symbole 
    symbole=false;
    for(i=0; i<nbSigFin; i++)
    {
        if(ligne[position]==sigFin[i])
        {
            ligne[position]=' ';
            position++;
            symbole=true;
            i=0;
        }
    }
    
    j=0;
    while(!(symbole) && (position < longLigne))
    {
        emplacement[j]=ligne[position];
        ligne[position]=' ';
        j++;
        position++;
        //arreter en presence "()[]-+%& \t*/;,.{}|!=\n\\?"
        for(i=0; i<nbSigFin; i++)
        {
            if(ligne[position]==sigFin[i])
            {
                symbole=true;
                break;
            }
        }
    }
    emplacement[j]='\0';
    //commence par un chiffre
    for(i=0; i<10; i++)
    { 
        if(emplacement[0]==chiffre[i])
        {
            emplacement[0]='\0';
            break;
        }
    }
    //mot sensiblement vide doit etre vider
    if((emplacement[0]==' ') || (emplacement[0]=='\t') || (emplacement[0]=='\n'))
    {
        emplacement[0]='\0';
    }
    
}
int insertMotcle(char **motCle)
{
    /*
        inserer les mot cles en c dans "char **motCle"
    */
    int i;

    i=0;
    strcpy(motCle[i], "int");
    i++;
    strcpy(motCle[i], "char");
    i++;
    strcpy(motCle[i],  "double");
    i++;
    strcpy(motCle[i], "float");
    i++;
    strcpy(motCle[i],"struct");
    i++;
    strcpy(motCle[i], "long");
    i++;
    strcpy(motCle[i], "if");
    i++;
    strcpy(motCle[i], "else");
    i++;
    strcpy(motCle[i], "for");
    i++;
    strcpy(motCle[i], "bool");
    i++;
    strcpy(motCle[i], "void");
    i++;
    strcpy(motCle[i], "typedef");
    i++;
    strcpy(motCle[i], "FILE");
    i++;
    strcpy(motCle[i], "unsigned");
    i++;
    strcpy(motCle[i], "default");
    i++;
    strcpy(motCle[i], "while");
    i++;
    strcpy(motCle[i], "do");
    i++;
    strcpy(motCle[i], "short");
    i++;
    strcpy(motCle[i], "printf");
    i++;
    strcpy(motCle[i], "scanf");
    i++;
    strcpy(motCle[i], "fprintf");
    i++;
    strcpy(motCle[i], "fscanf");
    i++;
    strcpy(motCle[i], "puts");
    i++;
    strcpy(motCle[i], "fputs");
    i++;
    strcpy(motCle[i], "gets");
    i++;
    strcpy(motCle[i], "fgets");
    i++;
    strcpy(motCle[i], "fgetc");
    i++;
    strcpy(motCle[i], "fopen");
    i++;
    strcpy(motCle[i], "fclose");
    i++;
    strcpy(motCle[i], "malloc");
    i++;
    strcpy(motCle[i], "free");
    i++;
    strcpy(motCle[i], "return");
    i++;
    strcpy(motCle[i], "main");
    i++;
    strcpy(motCle[i], "NULL");
    i++;
    strcpy(motCle[i], "true");
    i++;
    strcpy(motCle[i], "sizeof");
    i++;
    strcpy(motCle[i], "false");
    i++;
    strcpy(motCle[i], "strcat");
    i++;
    strcpy(motCle[i], "strstr");
    i++;
    strcpy(motCle[i], "strcpy");
    i++;
    strcpy(motCle[i], "strcmp");
    i++;
    strcpy(motCle[i], "strlen");
    i++;
    strcpy(motCle[i], "break");
    i++;
    strcpy(motCle[i], "exit");
    i++;
    strcpy(motCle[i], "case");
    i++;
    strcpy(motCle[i], "feof");
 
    return (i);
}