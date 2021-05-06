#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "game_io.h"

#define MAX 400

/**
  *@breaf mobiles représente l'ensembles des informations d'un monstre de la grille dont
  * l'abscisse, l'ordonnée, le monstre lui même ...
  */
typedef struct s_mobiles{
    int element;            // est un entier unique attribué à chaque monstre de la grille
    bool suivant;           // indique pour chaque monstre s'il pointe vers sont suivant (true) ou pas (false)
    int abscisse;           // colonne où se situe le monstre
    int ordonnees;          // ligne où se situe le monstre
    content monstre;
}mobiles;

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
 * @brief verifierMobile verifie si le tableau passé en paramètre contient un mobile
 * @param monstresMobile est le tableau les mobiles
 * @param taille est la taille de monstresMobile
 * @param position est un pointeur vers la position du mobile courant
 * @return l'indice du tableau où se situe le mobile ou -1 s'il n'ya plus de mobile
 */
int verifierMobile(mobiles* monstresMobile, int taille,int* position);
/**
 * @brief permute
 * @param mobile
 * @param voisin
 */
void permute(mobiles* mobile, mobiles* voisin);
/**
 * @brief changerDirection
 * @param monstresMobile
 * @param taille
 * @param m
 */
void changerDirection(mobiles monstresMobile[], int taille, int m);

bool verifierCache(mobiles solution[], mobiles cache [][MAX], int taille);

void afficherUnContent(content contenu);
void afficherGrille(game monJeu);
void viderBuffer();

typedef enum e_fonctions{FIND_ONE, NB_SOL, FIND_ALL}fonction;

int complexite = 1;

int main(int argc, char* argv[])
{
    //contrôle du nombre de paramètre de l'éxécutable
    //*
    if(argc!=4){
        fprintf(stderr,"undead_solve FIND_ONE|NB_SOL|FIND_ALL <nom_fichier_pb> <prefix_fichier_sol>\n");
        exit(EXIT_FAILURE);
    }
    game monJeu = load_game(argv[2]);
    FILE* fichier = NULL;
    fonction fonctionnalite;
    int nombreDeSolution = 0;
    if(strcmp(argv[1],"FIND_ONE")==0){
        fonctionnalite = FIND_ONE;
    }
    else if(strcmp(argv[1],"NB_SOL")==0)
    {
        fonctionnalite = NB_SOL;
    }
    else if(strcmp(argv[1],"FIND_ALL")==0){
        fonctionnalite = FIND_ALL;
        fichier = fopen("cache.sol","w");
        fclose(fichier);
    }
    else{
        fprintf(stderr,"Le deuxième paramètre doit être: FIND_ONE|NB_SOL|FIND_ALL\n");
        exit(EXIT_FAILURE);
    }

    int largeurGrille = game_width(monJeu);
    int hauteurGrille = game_height(monJeu);
    int nombreDeGhost = required_nb_monsters(monJeu, GHOST);
    int nombreDeVampire = required_nb_monsters(monJeu, VAMPIRE);
    int nombreDeZombie = required_nb_monsters(monJeu, ZOMBIE);
    int nombreDeSpirit = required_nb_monsters(monJeu, SPIRIT);

    bool identique = true;

    mobiles monstresMobile[nombreDeGhost+nombreDeVampire+nombreDeZombie+nombreDeSpirit];

    mobiles cache[nombreDeGhost+nombreDeVampire+nombreDeZombie+nombreDeSpirit][MAX];

    int k = 0;
    int cmpt = 1;
    for(int i=0; i<hauteurGrille; i++)
    {
        for(int j=0; j<largeurGrille; j++)
        {
            content courant = get_content(monJeu, j, i);
            if(courant == EMPTY){
                if(nombreDeGhost>0){
                    add_monster(monJeu, GHOST, j, i);
                    monstresMobile[k].element = cmpt++;
                    monstresMobile[k].suivant = false;
                    monstresMobile[k].abscisse = j;
                    monstresMobile[k].ordonnees = i;
                    monstresMobile[k].monstre = GHOST;
                    nombreDeGhost--;
                    k++;
                }
                else if (nombreDeVampire>0){
                    add_monster(monJeu, VAMPIRE, j, i);
                    monstresMobile[k].element = cmpt++;
                    monstresMobile[k].suivant = false;
                    monstresMobile[k].abscisse = j;
                    monstresMobile[k].ordonnees = i;
                    monstresMobile[k].monstre = VAMPIRE;
                    k++;
                    nombreDeVampire--;
                }
                else if (nombreDeZombie >0){
                    add_monster(monJeu, ZOMBIE, j, i);
                    monstresMobile[k].element = cmpt++;
                    monstresMobile[k].suivant = false;
                    monstresMobile[k].abscisse = j;
                    monstresMobile[k].ordonnees = i;
                    monstresMobile[k].monstre = ZOMBIE;
                    k++;
                    nombreDeZombie--;
                }
                else if (nombreDeSpirit>0){
                    add_monster(monJeu, SPIRIT, j, i);
                    monstresMobile[k].element = cmpt++;
                    monstresMobile[k].suivant = false;
                    monstresMobile[k].abscisse = j;
                    monstresMobile[k].ordonnees = i;
                    monstresMobile[k].monstre = SPIRIT;
                    k++;
                    nombreDeSpirit--;
                }
            }
        }
    }

    int n = cmpt-1;
    int m = n;
    int position = -1;

    for(int i=0; i<n; i++){
        switch(monstresMobile[i].monstre){
        case VAMPIRE:
            monJeu->squares[monstresMobile[i].abscisse][monstresMobile[i].ordonnees] = VAMPIRE;
            break;
        case ZOMBIE:
            monJeu->squares[monstresMobile[i].abscisse][monstresMobile[i].ordonnees] = ZOMBIE;
            break;
        case GHOST:
            monJeu->squares[monstresMobile[i].abscisse][monstresMobile[i].ordonnees] = GHOST;
            break;
        case SPIRIT:
            monJeu->squares[monstresMobile[i].abscisse][monstresMobile[i].ordonnees] = SPIRIT;
            break;
        default:
            fprintf(stderr,"le modele contient un monstre invalide\n");
            break;
        }
    }
    if(is_game_over(monJeu)){
        nombreDeSolution++;
    }
    int mallus = 1, positionMallus=n-1;
    bool gauche = true;
    bool suivant = false;
    do{
        help:
        if(mallus<n){
            position = positionMallus;
            if(gauche)
                positionMallus--;
            else
                positionMallus++;
        }else{
            m = verifierMobile(monstresMobile, n, &position);
            changerDirection(monstresMobile, n, monstresMobile[position].element);
            mallus = 0;
            if(gauche){
             positionMallus=0;
             gauche = false;
            }else{
             positionMallus=n-1;
             gauche = true;
            }
        }
        if(m == -1)
            break;
        mallus++;
        if(monstresMobile[position].suivant){
            permute(&monstresMobile[position], &monstresMobile[position+1]);
            suivant = true;
            if(monstresMobile[position].monstre == monstresMobile[position+1].monstre){
                goto help;
                complexite++;
                getchar();
            }
        }
        else
        {
            permute(&monstresMobile[position], &monstresMobile[position-1]);
            suivant = false;
            if(monstresMobile[position].monstre == monstresMobile[position-1].monstre){
                goto help;
                complexite++;
                getchar();
            }
        }
        // si le mobile est identique à son voisin, on ignore cette permutation
        if(!identique){
            monJeu->squares[monstresMobile[position].abscisse][monstresMobile[position].ordonnees] = monstresMobile[position].monstre;
            if(suivant){
                monJeu->squares[monstresMobile[position+1].abscisse][monstresMobile[position+1].ordonnees] = monstresMobile[position+1].monstre;
            }else{
                monJeu->squares[monstresMobile[position-1].abscisse][monstresMobile[position-1].ordonnees] = monstresMobile[position-1].monstre;
            }
            if(is_game_over(monJeu))    //si la combinaison correspond à une solution
            {
                switch(fonctionnalite){
                case FIND_ONE:
                    nombreDeSolution++;
                    m = -1;             // pour arrêter la recherche
                    break;
                case NB_SOL:
                    if(!identique && !verifierCache(monstresMobile, cache, n)){
                        nombreDeSolution++;
                    }
                    break;
                case FIND_ALL:
                    if(!identique && !verifierCache(monstresMobile, cache, n)){
                        nombreDeSolution++;
                        char nomFichier[strlen(argv[3])+8];
                        sprintf(nomFichier, "%s.sol%d", argv[3], nombreDeSolution);
                        save_game(monJeu, nomFichier);
                    }
                    break;
                }
            }
        }
        identique = false;

    }while(m!=-1);
    switch(fonctionnalite){
    case FIND_ONE:
        if(nombreDeSolution==1){
            char nomFichier[strlen(argv[3])+5];
            sprintf(nomFichier, "%s.sol", argv[3]);// si une solution a été trouvé, on enregistre la solution dans le fichier
            save_game(monJeu, nomFichier);
        }
        else if(nombreDeSolution==0){       // sinon, on écrit dans le fichier NO SOLUTION
            fichier = NULL;
            char nomFichier[strlen(argv[3])+5];
            sprintf(nomFichier, "%s.sol", argv[3]);
            fichier = fopen(nomFichier,"w");
            if(!fichier){
                fprintf(stderr, "Dans undead_solve: echec d'ouverture du fichier dans: case FIND_ONE\n");
                return EXIT_FAILURE;
            }
            fprintf(fichier, "NO SOLUTION");
            fclose(fichier);
        }
        break;
    case NB_SOL:{
        fichier = NULL;
        char nomFichier[strlen(argv[3])+7];
        sprintf(nomFichier, "%s.nbsol", argv[3]);
        fichier = fopen(nomFichier,"w");
        if(!fichier){
            fprintf(stderr, "Dans undead_solve: echec d'ouverture du fichier dans: case NB_SOL\n");
            return EXIT_FAILURE;
        }
        fprintf(fichier, "NB_SOL = %d", nombreDeSolution);  // on ecrit le nombre de solution (>=0)
        fclose(fichier);
    }
        break;
    case FIND_ALL:
        if(nombreDeSolution==0){
            fichier = NULL;
            fichier = fopen(argv[3],"w");
            if(!fichier){
                fprintf(stderr, "Dans undead_solve: echec d'ouverture du fichier dans: case FIND_ONE\n");
                return EXIT_FAILURE;
            }
            fprintf(fichier, "NO SOLUTION");
            fclose(fichier);
        }
        else{
            printf("%d solutions ont été ecrites dans un ou plusieurs fichiers\n", nombreDeSolution);
        }
        break;
    }
    printf("Traitement effectué avec succès...\n");
    return EXIT_SUCCESS;
}

int verifierMobile(mobiles* monstresMobiles, int taille,int* position)
{
    assert(monstresMobiles);
    int m = -1;
    for(int i=0;i<taille;i++){
        if(!monstresMobiles[i].suivant){
            if(i!=0){ // si c'est pas la prémière case du tableau
                if(monstresMobiles[i-1].element<monstresMobiles[i].element && monstresMobiles[i].element>m){
                    m=monstresMobiles[i].element;
                    *position = i;
                }
            }

        }else{
            if(i!=(taille-1)){
                if(monstresMobiles[i+1].element<monstresMobiles[i].element && monstresMobiles[i].element>m){
                    m=monstresMobiles[i].element;
                    *position = i;
                }
            }
        }

    }
    return m;
}

void permute(mobiles* mobile, mobiles* voisin){
    mobiles temp = *mobile;
    *mobile = *voisin;
    *voisin = temp;

    int abs = mobile->abscisse;
    mobile->abscisse = voisin->abscisse;
    voisin->abscisse = abs;

    int ord = mobile->ordonnees;
    mobile->ordonnees = voisin->ordonnees;
    voisin->ordonnees = ord;

    complexite++;
}
void changerDirection(mobiles tab[], int taille, int m){
    for(int i=0; i<taille; i++){
        if(tab[i].element>m && m!=tab[i].element)
        {
            if(tab[i].suivant)
                tab[i].suivant = false;
            else
                tab[i].suivant = true;
        }
    }
}

bool verifierCache(mobiles solution[], mobiles cache [][MAX], int taille)
{
    bool existe = true;
    bool static debut = true;
    static int compteurDeSolutions=0;

    // contrôle des paramètres
    if(!solution || !cache){
        fprintf(stderr, "paramètre invalide\n");
        exit(EXIT_FAILURE);
    }

    //verifier si l'élément éxiste
    for(int i=0; i<compteurDeSolutions; i++){
        for(int j=0; j<taille; j++){
            if(cache[j][i].monstre != solution[j].monstre){
                existe = false;
            }
        }
        if(existe){
            break;
        }else if(i!=(compteurDeSolutions-1)){
            existe = true;
        }
    }
    //si la solution n'existe pas, on l'enregistre dans le cache
    if(!existe || debut){
        for(int i=0; i<taille; i++){
            cache[i][compteurDeSolutions] = solution[i];
        }
        compteurDeSolutions++;;
        for(int j=0; j<taille; j++){
                //afficherUnContent(cache[j][compteurDeSolutions].monstre);
            }
        debut = false;
        return false;
    }
    return true;
}
