#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include"action.h"
#include"PointVirgule.h"

#define TAILLE_LIGNE 1000
#define NB_INST 1000
#define MAX_ERREUR 50

void testVir(char *fichier)
{
    /*
        vision des des lignes avec des virgules mal placee
        , abondant et aussi les lignes qui manquent de virgule
    */
    int num_ligne, nbInst, longLigne;
    int i, erreur;
    char *ligne, *tmp;
    bool instruction;
    int *tabInst;
    FILE *pf;

    //prelevement des lignes instructions dans tabInst
    tabInst=malloc(NB_INST*sizeof(int));
    nbInst=listerInst(fichier, tabInst);

    pf=fopen(fichier, "r");
    if(pf==NULL)
    {
        printf("erreur d'ouverture de fichier\n");
        exit(-1);
    }

    //allocation pour la manipulation des lignes 
    ligne=malloc(TAILLE_LIGNE*sizeof(char));
    tmp=malloc(TAILLE_LIGNE*sizeof(char));
    num_ligne=1;
    while(!(feof(pf)))
    {
        fgets(ligne, TAILLE_LIGNE, pf);
        //Si c'est une ligne instruction
        //num_ligne doit se trouver dans le tableau tabInst
        instruction=false;
        for(i=0; i<nbInst; i++)
        {
            if(num_ligne==tabInst[i])
            {
                instruction=true;
                break;
            }
        }
        /*
            copier la ligne dans tmp pour le visionner
            puis afficher la ligne en cas d' erreur
        */
        strcpy(tmp, ligne);
        
        // enlever les guillements puis les commentaires
        repareLigne(tmp, pf);
        // identifier l'erreur en precisant 
        // a la fonction trouveErreur que c'est une instruction ou pas
        erreur=trouveErreur(tmp, instruction);
        switch (erreur)
        {
            case 1 :
            {
                printf("ligne n°%d :\n",num_ligne);
                printf("--> %s manque de virgule\n\n",ligne);
                //compteur de numero de ligne
                num_ligne++;
                break;
            }
            case 2 :
            {
                printf("ligne n°%d :\n",num_ligne);
                printf("--> %s trop virgule !\n\n",ligne);
                //compteur de numero de ligne
                num_ligne++;
                break;
            }
            case 3 :
            {
                printf("ligne n°%d :\n",num_ligne);
                printf("--> %s place de virgule incorrecte !\n\n",ligne);
                //compteur de numero de ligne
                num_ligne++;
                break;
            }
            default :
            {
                num_ligne++;
                break;
            }
        }
    }

    free(ligne);
    free(tmp);
    free(tabInst);
    fclose(pf);
}
int listerInst(char *fichier, int *nLigne)
{
    //recuperation des num_ligne instructions de fichier dans nLigne

    int taille, compteur, num_ligne, longLigne;
    int i, j, apresStruct;
    bool instruction;
    bool accollade, structu;
    char *ligne, *ligneSuiv, *ligneAv;
    FILE *ptrFic;
    char typStruc[8];

    ligne=malloc(TAILLE_LIGNE*sizeof(char));
    ligneSuiv=malloc(TAILLE_LIGNE*sizeof(char));
    ligneAv=malloc(TAILLE_LIGNE*sizeof(char));
    strcpy(typStruc, "struct ");
    //taille du tableau initialement 0
    taille=0;
    //pour compter le nombre de ligne , initialement n°1
    compteur=1;
    num_ligne=1;
    ptrFic=fopen(fichier, "r");
    if(ptrFic==NULL)
    {
        printf("erreur d'ouverture de fichier\n");
        exit(-1);
    }
    //initialisation
    instruction=true;
    ligneAv[0]='\0';
    ligne[0]='\0';
    ligneSuiv[0]='\0';
    structu=false;
    apresStruct=0;
    fgets(ligne, TAILLE_LIGNE, ptrFic);
    while(!(feof(ptrFic)))
    {
        i=0;
        while( (ligneAv[i]==' ') || (ligneAv[i]=='\t') )
        {
            i++;
        }
        if((ligneAv[i]=='{') || (ligneAv[i]=='}'))
        {
            instruction=true;
        }
        //ignorer les directives preprocesseurs
        i=0;
        while(ligne[i]==' ' || ligne[i]=='\t')
        {
            i++;
        }
        while(ligne[i]=='#')
        {
            fgets(ligne, TAILLE_LIGNE, ptrFic);
            compteur++;
        }
        num_ligne=compteur;
        
        // enlever les guillements puis les commentaires
        longLigne=repareLigne(ligne, ptrFic);

        //  une ligne ::: Simple accollade    //
        i=0;
        while( ligne[i]==' ' || ligne[i]=='\t' )
        {
            i++;
        }
        if((ligne[i]=='{') || (ligne[i]=='}'))
        {
            i++;
            while( ligne[i]==' ' || ligne[i]=='\t' )
            {
                i++;
            }
            if(ligne[i]=='\n')
            {
                instruction=false;
            }
        }
        
        //detection des structures en declaration
        i=0;
        while( ligne[i]==' ' || ligne[i]=='\t' )
        {
            i++;
        }
        if((ligne[i]=='}') && (apresStruct!=0))
        {
            instruction=true;
            apresStruct--;
        } 
        
        //pour une ligne  qui se termine par une accollade " OUVERT "
        longLigne=strlen(ligne);
        i=(longLigne-2);
        while( (ligne[i]==' ') || (ligne[i]=='\t') || (ligne[i]==';') )
        {
            i--;
        }
        if((ligne[i]=='{'))
        {
            accollade=true;
            instruction=false;
        }
        //remesurer vue le teste de commentaire
        longLigne=strlen(ligne);
        //en cas de ligne commentaire , il sera vide donc different de instruction
        i=0;
        while( ligne[i]==' ' || ligne[i]=='\t' )
        {
            i++;
        }
        if(ligne[i]=='\n')
        {
            instruction=false;
        }
        //si  ligne suivant commence par "{" , ce ligne ne doit pas etre une instruction
        fgets(ligneSuiv, TAILLE_LIGNE, ptrFic);
        i=0;
        while( ligneSuiv[i]==' ' || ligneSuiv[i]=='\t' )
        {
            i++;
            if(ligneSuiv[i]=='\n')
            {
                fgets(ligneSuiv, TAILLE_LIGNE, ptrFic);
                compteur++;
                i=0;
            }
        }
        if(ligneSuiv[i]=='{')
        {
            accollade=true;
            instruction=false;
        } 
        
        // Pour les declaration des structure 
        structu=false;
        if(longLigne>7)
        {
            //commence par struct
            i=0;
            while( (ligne[i]==' ') || (ligne[i]=='\t'))
            {
                i++;
            }
            j=0;
            while((ligne[i]==typStruc[j]))
            {
                if(j==7)
                {
                    structu=true;
                }
                i++;
                j++;
                if(typStruc[j]=='\0')
                {
                    break;
                }
            }
        }
        if((structu) && (accollade))
        {
            apresStruct++;
        }
        //enregistrer le numero de ligne si c'est une instruction
        if(instruction)
        {
            nLigne[taille]=num_ligne;
            taille++;
        }
        strcpy(ligneAv, ligne);
        strcpy(ligne, ligneSuiv);
        //reconsideration
        accollade=false;
        instruction=true;
        //compteur de ligne
        compteur++;
    }

    fclose(ptrFic);
    free(ligne);
    free(ligneSuiv);
    free(ligneAv);

    return (taille);
}
int trouveErreur(char *ligne, bool instruction)
{
    /*
        valeur de retour :
            0 = sans erreur
            1 = manque de virgule
            2 = trop de virgule
            3 = place incorrecte de virgule
    */
    int resultat, nbVirgule;
    int i, longLigne;
    bool Efor;
    char *enchaine;

    longLigne = strlen(ligne);
    resultat = 0;
    //compter le nombre de virgule
    nbVirgule=0;
    for(i=0; i<longLigne; i++)
    {
        if(ligne[i]==';')
        {
            nbVirgule++;
        }
    }
    //saut des espace et tabulation ou les innattendu comme { suivi d'instruction
    i=0;
    while((ligne[i]==' ') || (ligne[i]=='\t') )
    {
        i++;
    }
    //ligne commence par un ';'
    if((ligne[i]==';')) 
    {
        resultat = 3;
    }
    //Vision de la fin des phrases
    //ligne[(longLigne-1)] doit etre le '\n', donc voyons depuis (longLigne-2)
    i=(longLigne-2);
    while((ligne[i]==' ') || (ligne[i]=='\t') || (ligne[i]=='{'))
    {
        i--;
    }
    //instruction mais manque de virgule
    if(instruction && (ligne[i]!=';'))
    {
        resultat = 1;
    }
    //instruction mais trop de virgule
    if(instruction && (ligne[i]==';') )
    {
        //deux virgule a la fin de phrase
        i--;
        while((ligne[i]==' ') || (ligne[i]=='\t'))
        {
            i--;
        }
        if((ligne[i]==';'))
        {
            resultat = 2;
        }
    }
    //ligne non instruction mais se finisse par une virgule
    if( !(instruction) && (ligne[i]==';')) 
    {
        resultat = 3;
    }
    //etude des point virgule de la ligne comme for( ; ; ) 
    Efor=false;
    i=0;
    while((ligne[i]==' ') || (ligne[i]=='\t'))
    {
        i++;
    }
    if(i<(longLigne-2))
    {
        if((ligne[i]=='f') && (ligne[i+1]=='o') && (ligne[i+2]=='r'))
        {
            i=i+3;
            while((ligne[i]==' ') || (ligne[i]=='\t'))
            {
                i++;
            }
            if(ligne[i]=='(')
            {
                //signaler la presence de " for(" en debut de phrase
                Efor=true;
            }
        }
    }
    //enchaine = trouver dans ligne ";;" ou "; ;"*
    enchaine=NULL;
    if(nbVirgule>1)
    { 
        for(i=0; i<longLigne; i++)
        {
            if(ligne[i]==';')
            {
                i++;
                while((ligne[i]==' ') || (ligne[i]=='\t'))
                {
                    i++;
                }
                if(ligne[i]==';')
                {
                    //affecter une addresse pour qu'il soit non null
                    enchaine=strchr(ligne, ';');
                }
            }
        }
    }
    if(enchaine!=NULL)
    {
        resultat = 2;
    }
    
    if(Efor && (nbVirgule!=2))
    {
        if(nbVirgule < 2)
        {
            resultat=1;
        }
        else if(nbVirgule > 2)
        {
            resultat=3;
        }
    }
    
    return (resultat);
}