#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "game_io.h"

#define MAX             1000           //Taille maximale d'une ligne dans le fichier
#define NOMBRE_MONSTRES    4

/**
 *@brief représente les different monstres du jeu
*/
typedef struct monstres_s{
    int ghost;
    int vampire;
    int zombie;
    int spirit;
} monstres;

/**
 * @brief The game_s struct est la principale structure du jeu
 */
struct game_s{
    content **squares;              //tableau de content
    monstres nb_monstre_requis;     //nombre de monstre requis
    monstres nb_current_monster;    //nombre de monstres courant
    int **labels;
    int height;
    int width;
};

/**
  *@brief the reperLigne reperesente chaque ligne dans le fichier (sauf les lignes pour les tableau de content
  */
typedef enum repereLigne_e{DIMENSIONS, NOMBRE_DE_MONSTRES_REQUIT, LABEL_N, LABEL_S, LABEL_E, LABEL_W} repereLigne;


/**
 * @brief nullPointer afficher un message et quitte l'éxécution du programme
 * @param message est un pointeur sur le message à afficher
 */
void nullPointer(char* message);
/**
 * @brief convert_line convertie une chaine de caractère separée par des espaces en un tableau de long
 * @param line est la chaine de caractère à convertir
 * @param p_size est un pointeur vers la taille du nouveau tableau de long
 * @return un tableau de long
 */
long* convert_line(char* line,long* p_size);
/**
 * @brief read_next_line lit une ligne dans un fichier
 * @param p_f est le pointeur vers le fichier dont on veut lire le contenu
 * @param p_size est la taille de la chaine de caractére (sera modifiée par la fonction)
 * @return une chaine de caractère
 */
char* read_next_line(FILE* p_f, long* p_size);

game load_game(char* filename)
{
    //contrôle du paramètre
    if(!filename){
        nullPointer("Dans load_game: paramètre invalide");
    }
    game g = NULL;
    FILE* fichier = NULL;
    fichier = fopen(filename, "r"); //ouverture du fichier
    if(!fichier){
        fprintf(stderr,"Dans load_game: l'ouverture du fichier %s a echoué\n", filename);
        exit(EXIT_FAILURE);
    }
    long size=0;
    char* ligne = NULL;             // contiendra une ligne lu dans le fichier
    long* chaineConvertie = NULL;   // contiendra une ligne du fichier convertie en long
    repereLigne repere = 0;         // reperer sur quelle ligne on est dans le fichier
    int hauteur = 0;                // sera utiliser pour lire les contents (case default) en commençant à la dernière ligne
    int colonne = 0;
    int k = 0;                      // est utilisé lors du parcours de la variable ligne dans le case default
    do
    {
        ligne = read_next_line(fichier, &size);
        if(!ligne)
            nullPointer("Dans load_game: pointeur null");
        if(repere<=LABEL_W)
        {
            chaineConvertie = convert_line(ligne, &size);
            if(!chaineConvertie)
                nullPointer("Dans load_game: pointeur null lors du chargement des dimensions");
        }
        switch (repere) {
        case DIMENSIONS:    //prémière ligne du fichier
            if(size!=2)
            {
                fprintf(stderr, "Dans load_game: le format du fichier est invalide (il doit y avoir exactement deux dimensions\n");
                exit(EXIT_FAILURE);
            }
            hauteur = chaineConvertie[1]-1;
            g = new_game_ext(chaineConvertie[0], chaineConvertie[1]);
            break;
        case NOMBRE_DE_MONSTRES_REQUIT: //deuxième ligne du fichier
            if(size!=NOMBRE_MONSTRES)
            {
                fprintf(stderr, "Dans load_game: le format du fichier est invalide (il doit y avoir 4 entiers)\n");
                exit(EXIT_FAILURE);
            }
            g->nb_monstre_requis.ghost = chaineConvertie[1];
            g->nb_monstre_requis.vampire = chaineConvertie[0];
            g->nb_monstre_requis.zombie = chaineConvertie[2];
            g->nb_monstre_requis.spirit = chaineConvertie[3];
            break;
        case LABEL_N:
            if(size!=g->width){
                fprintf(stderr, "Dans load_game: le format du fichier est invalide (label nord invalide)\n");
                exit(EXIT_FAILURE);
            }
            for(int i=0; i<g->width; i++){
                g->labels[N][i] = chaineConvertie[i];
            }
            break;
        case LABEL_S:
            if(size!=g->width){
                fprintf(stderr, "Dans load_game: le format du fichier est invalide (label sud invalid)\n");
                exit(EXIT_FAILURE);
            }
            // parcours de la ligne lu dans le fichier et affectation des valeurs dans g->labels
            for(int i=0; i<g->width; i++){
                g->labels[S][i] = chaineConvertie[i];
            }
            break;
        case LABEL_E:
            if(size!=g->height){
                fprintf(stderr, "Dans load_game: le format du fichier est invalide (label ESt invalide)\n");
                exit(EXIT_FAILURE);
            }
            // parcours de la ligne lu dans le fichier et affectation des valeurs dans g->labels
            for(int i=0; i<g->height; i++){
                g->labels[E][i] = chaineConvertie[i];
            }
            break;
        case LABEL_W:
            if(size!=g->height){
                fprintf(stderr, "Dans load_game: le format du fichier est invalide (label West invalide)\n");
                exit(EXIT_FAILURE);
            }
            // parcours de la ligne lu dans le fichier et affectation des valeurs dans g->labels
            for(int i=0; i<g->height; i++){
                g->labels[W][i] = chaineConvertie[i];
            }
            break;
        default:        //cas pour les contents (monstres et mirroirs)
            colonne = 0;
            k=0;
            while(ligne[k]!='\0'){
                switch(ligne[k]){
                case 'V':
                    g->squares[colonne++][hauteur] = VAMPIRE;
                    break;
                case 'G':
                    g->squares[colonne++][hauteur] = GHOST;
                    break;
                case 'Z':
                    g->squares[colonne++][hauteur] = ZOMBIE;
                    break;
                case 'S':
                    g->squares[colonne++][hauteur] = SPIRIT;
                    break;
                case '/':
                    g->squares[colonne++][hauteur] = MIRROR;
                    break;
                case '\\':
                    g->squares[colonne++][hauteur] = ANTIMIRROR;
                    break;
                case '|':
                    g->squares[colonne++][hauteur] = VMIRROR;
                    break;
                case '-':
                    g->squares[colonne++][hauteur] = HMIRROR;
                    break;
                case '.':
                    g->squares[colonne++][hauteur] = EMPTY;
                    break;
                default:
                    break;
                }
                k++;
            }
            hauteur--;
            break;
        }
        repere++;
        if(repere<=LABEL_W+1)
            free(chaineConvertie);
        free(ligne);
    }while(ligne!=NULL && repere<g->height + 6);
    fclose(fichier);    // fermeture du fichier
    return g;
}

void save_game(cgame g, char* filename)
{
    if(!g || !filename){
        fprintf(stderr,"Dans save_game: paramètre invalide\n");
        exit(EXIT_FAILURE);
    }
    FILE *fichier = fopen(filename, "w");
    if(!fichier){
        fprintf(stderr,"Dans save_game: impossible d'ouvrir le fichier\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fichier, "%d %d\n", game_width(g), game_height(g));         // ecriture des dimensions:

    fprintf(fichier,"%d %d %d %d\n", g->nb_monstre_requis.vampire, g->nb_monstre_requis.ghost, g->nb_monstre_requis.zombie, g->nb_monstre_requis.spirit);   // ecriture du nombre de monstres requis
    for(int i=0; i<g->width; i++){
        fprintf(fichier,"%d ", g->labels[N][i]);    // ecriture du label nord
    }
    fprintf(fichier, "\n");
    for(int i=0; i<g->width; i++){
        fprintf(fichier,"%d ", g->labels[S][i]);    // ecriture du label sud
    }
    fprintf(fichier, "\n");
    for(int i=0; i<g->height; i++){
        fprintf(fichier,"%d ", g->labels[E][i]);    // ecriture du label Est
    }
    fprintf(fichier, "\n");
    for(int i=0; i<g->height; i++){
        fprintf(fichier,"%d ", g->labels[W][i]);    // ecriture du label West
    }
    fprintf(fichier, "\n");
    /*ecriture des contents:
     * on commence à parcourir la derniére ligne (hauteur-1)
     * pour chaque ligne on parcours toutes les colonnes et on ecrit le content
     * l'ecriture de chaque ligne est terminée par un \n
     */
    int largeur = game_width(g);
    int hauteur = game_height(g);
    for(int i=hauteur-1; i>=0; i--)     //parcours les lignes
    {
        for(int j=0; j<largeur; j++)    //parcours les colonnes
        {
            switch(g->squares[j][i]){
            case VAMPIRE:
                fprintf(fichier, "V ");
                break;
            case ZOMBIE:
                fprintf(fichier, "Z ");
                break;
            case GHOST:
                fprintf(fichier, "G ");
                break;
            case SPIRIT:
                fprintf(fichier, "S ");
                break;
            case MIRROR:
                fprintf(fichier, "/ ");
                break;
            case ANTIMIRROR:
                fprintf(fichier, "\\ ");
                break;
            case VMIRROR:
                fprintf(fichier, "| ");
                break;
            case HMIRROR:
                fprintf(fichier, "- ");
                break;
            case EMPTY:
                fprintf(fichier, ". ");
                break;

            }
        }
        fprintf(fichier, "\n");
    }
    fclose(fichier);                    // fermeture du fichier
}

char* read_next_line(FILE* pointeurSurFichier, long* taille){
    char* line = (char*)malloc(MAX*sizeof(char));
    if(line==NULL)
    {
        fprintf(stderr,"Error memory\n");
        exit(EXIT_FAILURE);
    }
    long anciennePosition = ftell(pointeurSurFichier);
    long longuer = 0;
    if(fgets(line,MAX,pointeurSurFichier)!=NULL)
    {
        longuer = ftell(pointeurSurFichier)-anciennePosition;
        if(line[longuer-1]=='\n'){
            line[longuer-1]='\0';
            longuer--;
        }else{
            if(!feof(pointeurSurFichier)){
                fprintf(stderr,"Line too long...\n");
                exit(EXIT_FAILURE);
            }
        }
        (*taille)=longuer;
        return line;
    }
    return NULL;
}

long* convert_line(char* ligne,long* taille){
    long* tableau = malloc((*taille)*sizeof(long));
    if(tableau == NULL){
        fprintf(stderr,"Allocation impossible\n");
        exit(EXIT_FAILURE);
    }
    long arr_s=0;
    char * token = strtok(ligne," ");
    while(token != NULL){
        char* end_value = token;
        long val = strtol(token, &end_value, 10);
        if((*end_value)=='\0'){
            tableau[arr_s]=val;
            arr_s++;
        }else{
            free(tableau);
            return NULL;
        }
        token = strtok(NULL, " ");
    }
    (*taille)=arr_s;
    return tableau;
}

void nullPointer(char* message){
    fprintf(stderr,"%s\n", message);
    exit(EXIT_FAILURE);
}
