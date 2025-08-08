#include<stdlib.h>
#include<string.h>
#include"PointVirgule.h"
#include"detectVar.h"
#include"action.h"

#define MOT 50

void execute(int nbPara, char *argu[])
{
    char **optionValide;
    int i, j, test, nbOption;

    //definition des options valide avec l'allocation de leur place
    nbOption = 4;
    optionValide=malloc(nbOption*sizeof(char *));
    for(i=0; i<nbOption; i++)
    {
        optionValide[i]=malloc(MOT*sizeof(char));
    }
    strcpy(optionValide[0], "--aide");
    strcpy(optionValide[1], "-p");
    strcpy(optionValide[2], "-v");
    strcpy(optionValide[3], "-pv");

    //definition des actions a faire
    if(nbPara < 2)
    {
        afficheAide(argu[0]);
        exit (1);
    }
    //tester si l'option est valide
    for(i=0; i<nbOption; i++)
    {
        test=strcmp(argu[1], optionValide[i]);
        if(test==0)
        {
            break;
        }
    }
    //sortir si l'option n'est pas valable
    if(test!=0)
    {
        printf("\tOption non valide !\n\n");
        afficheAide(argu[0]);
        exit (1);
    }
    
    //l'option est identifier en "i eme position" 
    switch (i)
    {
        case 0:
        {
            //"--aide" : demande d'aide
            afficheAide(argu[0]);
            break;
        }
        case 1:
        {
            //"-p" : identifier les points virgules
            testVir(argu[2]);
            break;
        }
        case 2:
        {
            //-v : identifier les variables
            variable(argu[2]);
            break;
        }
        case 3:
        {
            //-pv : identifier les variables et points virgules
            testVir(argu[2]);
            variable(argu[2]);
            break;
        }
    }  
}
int repareLigne(char *ligne_c, FILE* pf_c)
{
    /*
        ignorer les commentaires
        petit modification de ligne pour eviter l'erreur printf("//non comm\n");
        suppression de l'entreGUIMENT
    */

    int longLigne, posGUI, i, j, decalage;
    char avant, caractere;
    bool finComm;
    char *comm;

    longLigne=strlen(ligne_c);
    for(i=0; i<=longLigne; i++)
    {
        if(ligne_c[i]=='"')
        {
            posGUI=i;
            decalage=0;
            do
            {
                i++;
                decalage++;
            }
            while(ligne_c[i]!='"');
            for(j=posGUI; j<(longLigne-(decalage)); j++)
            {
                ligne_c[j]=ligne_c[(j+decalage+1)];
            }
            i=i-decalage;
        }
    }
    //enlever les cotes comme '\n' ou '\t',
    longLigne=strlen(ligne_c);
    for(i=0; i<=longLigne; i++)
    {
        if(ligne_c[i]=='\'')
        {
            posGUI=i;
            decalage=0;
            do
            {
                i++;
                decalage++;
            }
            while(ligne_c[i]!='\'');
            for(j=posGUI; j<(longLigne-(decalage)); j++)
            {
                ligne_c[j]=ligne_c[(j+decalage+1)];
            }
            i=i-decalage;
        }
    }
    comm=strstr(ligne_c, "//");
    if(comm!=NULL)
    {
        for(i=0; i<(longLigne-1); i++)
        {
            if(ligne_c[i]=='/' && ligne_c[i+1]=='/')
            {
                ligne_c[i]='\n';
                ligne_c[i+1]='\0';
                break;
            }
        }
    }
    comm=strstr(ligne_c, "/*");
    if(comm!=NULL)
    {
        comm=strstr(ligne_c, "*/");/* EX : comm */
        for(i=0; i<(longLigne-1); i++)
        {
            if(ligne_c[i]=='/' && ligne_c[i+1]=='*')
            {
                ligne_c[i]='\n';
                ligne_c[i+1]='\0';
                break;
            }
        }
        if(comm==NULL)
        {
            avant=fgetc(pf_c);
            do
            {
                caractere=fgetc(pf_c);
                if((feof(pf_c)))
                {
                    break;
                }
                if(avant=='*' && caractere=='/')
                {
                    finComm=true;
                }
                    else
                    {
                        finComm=false;
                    }
                avant=caractere;
            }
            while(!(finComm));
        }
    }
    longLigne=strlen(ligne_c);

    return longLigne;
}
void afficheAide(char *para)
{
    printf("  Utilisation :\t%s  [ option ] < fichier_c > \n", para);
    printf("Option :\n");
    printf("\t--aide : Affiche cette aide\n");
    printf("\t-p     : Affiche les erreurs concernants les virgules\n");
    printf("\t-v     : Affiche les erreurs concernants les variables\n");
    printf("\t-pv     : Affiche les erreurs concernants les variables et les virgules\n");
}