#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "game.h"

#define TAILLE_GRILLE_FIXE                  4   // sert de taille pour l'ancienne version
#define INDICE_MINIMUM                      0   // represente l'indice minimum sur une ligne ou sur une colonne
#define NOMBRE_MONSTRE_COURANT_INITIAL      0
#define NOMBRE_MONSTRE_REQUIS_INITIAL       0

/**
 *@brief directionRayon permet de representer la direction d'un rayon dans la grille
 */
typedef enum directionRayon_e{HAUT, BAS, GAUCHE, DROITE} directionRayon;

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
 * @brief determinerCaseSuivante determine la prochaine case à checker lors
 * de la recherche du nombre de monstres courant pouvant être vu
 * @param contenu est le content qui va indiquer si on doit changer de direction (HMIRROR, MIRROR ...)
 * ou pas (ZOMBIE, VAMPIRE ...)
 * @param directionCourante est le sens vers lequel le rayon se dirige ( HAUT, BAS, GAUCHE, DROITE)
 * @param ligne represente une ligne de la grille
 * @param colonne reprensente une colonne de la grille
 */
void determinerCaseSuivante(content contenu, directionRayon *directionCourante, int* ligne, int* colonne);

/**
 * @brief actualiserNombreDeMonstres actualise la variable nombreDeMonstres a chaque fois
 * qu'un le content passé en paramètre (pour le cas des monstres) peut être vu
 * a une position donnée (colonne, ligne)
 * @param contenu est le content a checker:
 * si c'est un mirroir( MIRROR, ANTIMIRROR, VMIRROR, HMIRROR), la fonction mettra seulement à jour "mirroirDectecte"
 * si c'est un monstre quelconque, la fonction devra actualiser "nombreDeMonstres" en ajoutant 0 (SPIRIT) ou 1
 * @param directionCourante est le sens vers lequel le rayon se dirige ( HAUT, BAS, GAUCHE, DROITE)
 * @param mirroirDetecte sera mis à true si on un mirroir a été detecté lors du parcours
 * @param nombreDeMonstres represente le nombre de monstre vu lors du parcours
 */
void actualiserNombreDeMonstres(content contenu,directionRayon directionCourante,bool *mirroirDetecte,int* nombreDeMonstres);

/**
 * @brief erreurAllocation affiche un message et quitte l'execution du programme
 */
void erreurAllocation();


game new_game()
{
    //on utilise new_game_ext en lui passant la même dimension comme largeur et hauteur
    game monJeu = new_game_ext(TAILLE_GRILLE_FIXE,TAILLE_GRILLE_FIXE);
    return monJeu;
}
void add_mirror(game game, int dir, int col, int line)
{
    //verification de la validité du pointeur
    if(!game)
    {
        fprintf(stderr,"pointeur null dans  add_mirror\n");
        exit(EXIT_FAILURE);
    }
    // verification de la validité du mirroir et appelle de "add_mirror_ext" selon
    // le type de mirroir
    switch (dir){
    case 0:
        add_mirror_ext(game, MIRROR, col, line);
        break;
    case 1:
        add_mirror_ext(game, ANTIMIRROR, col, line);
        break;
    default:
        fprintf(stderr,"Dans add_mirror: le paramètre dir prend 0 ou 1\n");
        return;
        break;
    }
}
void set_required_nb_seen(game game, direction side, int pos, int value)
{
    //verification de la validité des paramètres
    if(!game)
    {
        fprintf(stderr,"pointeur null dans set_required_nb_seen\n");
        exit(EXIT_FAILURE);
    }
    if(value<0 || pos<INDICE_MINIMUM)
    {
        fprintf(stderr,"Le param pos et value ne peuvent pas être négatif\n");
        return;
    }
    //on contrôle le paramètre pos en fonction des cotés
    switch(side)
    {
    case N: // les labels côté nord et sud dependent de la largeur de la grille
    case S:
        if(pos>=game->width)
        {
            fprintf(stderr,"Dans set_required_nb_seen: le paramètre pos doit être inférieur à %d",game->width);
            return;
        }
        break;
    case E: // les labels côté Est et Ouest dependent de la hauteur de la grille
    case W:
        if(pos>=game->height)
        {
            fprintf(stderr,"Dans set_required_nb_seen: le paramètre pos doit être inférieur à %d",game->height);
            return;
        }
        break;
    default:
        fprintf(stderr,"le parmètre side est invalide: N|S|E|W\n");
        return;
    }
    // à partir de la tout est ok
    game->labels[side][pos]=value;
}

void set_required_nb_monsters(game game, content monster, int value)
{
    //verification de la validité des paramètres
    if(!game)
    {
        fprintf(stderr,"le parametre game de set_riquired_nb_monstre ne doit pas être null\n");
        exit(EXIT_FAILURE);
    }
    if(value<0)
    {
        fprintf(stderr,"Dans set_required_nb_monsters, l'argument value ne peut pas être négatif\n");
        return;
    }
    /*on affecte value a "nombre_monstre_requis" si le monstre est valide
     * a defaut le traitement est ignoré
     */
    switch (monster) {
    case ZOMBIE:
        game->nb_monstre_requis.zombie=value;
        break;
    case VAMPIRE:
        game->nb_monstre_requis.vampire=value;
        break;
    case GHOST:
        game->nb_monstre_requis.ghost=value;
        break;
    case SPIRIT:
        game->nb_monstre_requis.spirit=value;
        break;
    default:
        fprintf(stderr, "Dans set_required_nb_monsters, l'argument monstre doit être un ZOMBIE|GHOST|VAMPIRE|SPIRIT\n");
        return;
        break;
    }
}

game copy_game(cgame g_src)
{
    //on crée le nouveau jeu à clôner en fonction des dimension de g_src
    game g = new_game_ext(g_src->width, g_src->height);
    //on copie le contenu de la grille du jeu source dans le nouveau jeu crée
    for(int i=0;i<g->width;i++)
    {
        for(int j=0;j<g->height;j++)
        {
            g->squares[i][j]=g_src->squares[i][j];
        }
    }
    //pour les labels nord et sud,on fait la copie en parcourant en fonction de la largeur de la grille
    for(int j=0; j<g->width; j++) {
        g->labels[N][j] = g_src->labels[N][j];
        g->labels[S][j] = g_src->labels[S][j];
    }
    // pour les labels est et ouest, on fait la copie en parcourant en fonction de la hauteur de la grille
    for(int j=0; j<g->height; j++) {
        g->labels[E][j] = g_src->labels[E][j];
        g->labels[W][j] = g_src->labels[W][j];
    }
    // clônage du nombre de monstes requis
    g->nb_monstre_requis.zombie = g_src->nb_monstre_requis.zombie;
    g->nb_monstre_requis.vampire = g_src->nb_monstre_requis.vampire;
    g->nb_monstre_requis.ghost = g_src->nb_monstre_requis.ghost;
    g->nb_monstre_requis.spirit = g_src->nb_monstre_requis.spirit;
    // clônage du nombre de monstres courant
    g->nb_current_monster.zombie = g_src->nb_current_monster.zombie;
    g->nb_current_monster.vampire = g_src->nb_current_monster.vampire;
    g->nb_current_monster.ghost = g_src->nb_current_monster.ghost;
    g->nb_current_monster.spirit = g_src->nb_current_monster.spirit;

    return g;
}

void delete_game(game g)
{
    if(g!=NULL)
    {
        // liberer les sous tableaux de content
        for(int i=0; i<g->width; i++)
        {
            if(g->squares[i]!=NULL){
                free(g->squares[i]); // libération des tableau de content
            }
        }
        // liberer les sous tableaux de labels
        for(int i=0; i<NB_DIR; i++)
        {
            if(g->labels[i]!=NULL){
                free(g->labels[i]); // libération des tableau de labels
            }
        }
        //on libere maintenant les tableau principaux et la structure principal du jeu
        free(g->squares);
        free(g->labels);
        free(g);
    }
}

int required_nb_seen(cgame game, direction side, int pos)
{
    // verification de la validité des paramètres
    if(!game)
    {
        fprintf(stderr,"Dans required_nb_seen, le parametre game ne doit pas être null\n");
        exit(EXIT_FAILURE);
    }
    if(pos<INDICE_MINIMUM)
    {
        fprintf(stderr, "DAns required_nb_seen: Le param pos de required_nb_seen ne doit pas être negatif\n");
        exit(EXIT_FAILURE);
    }
    //verificatio du paramètre pos en fonction de des côtés
    switch(side)
    {
    case N: // N e S dependent de la largeur
    case S:
        if(pos>=game->width)
        {
            fprintf(stderr,"Dans required_nb_seen: le paramètre pos doit être inférieur à %d",game->width);
            exit(EXIT_FAILURE);
        }
        break;
    case E: // E et W dependent de la hauteur
    case W:
        if(pos>=game->height)
        {
            fprintf(stderr,"Dans required_nb_seen: le paramètre pos doit être inférieur à %d",game->height);
            exit(EXIT_FAILURE);
        }
        break;
    default:
        fprintf(stderr,"Dans required_nb_seen: le paramètre side doit être N|S|E|W\n");
        exit(EXIT_FAILURE);
        break;
    }
    return game->labels[side][pos];
}

content get_content(cgame game, int col, int line)
{
    if(!game)
    {
        fprintf(stderr,"Dans get_content: pointeur null dans get_content\n");
        exit(EXIT_FAILURE);
    }
    // tester si la position (colonne et ligne) sont valides
    if(col<INDICE_MINIMUM || col>game->width-1 || line<INDICE_MINIMUM || line>game->height-1)
    {
        fprintf(stderr,"Dans get_content: les paramètres col et/ou line sont invalides\n");
        exit(EXIT_FAILURE);
    }

    return game->squares[col][line];
}

int required_nb_monsters(cgame game, content monster)
{
    if(!game)
    {
        fprintf(stderr,"le parametre game de riquired_nb_monstre ne doit pas être null\n");
        exit(EXIT_FAILURE);
    }
    int nombreDeMonstres=0;
    // recuperer le nombre de monstres si "monster" est valide, sinon le traitement est ignoré
    switch (monster) {
    case ZOMBIE:
        nombreDeMonstres = game->nb_monstre_requis.zombie;
        break;
    case VAMPIRE:
        nombreDeMonstres = game->nb_monstre_requis.vampire;
        break;
    case GHOST:
        nombreDeMonstres = game->nb_monstre_requis.ghost;
        break;
    case SPIRIT:
        nombreDeMonstres = game->nb_monstre_requis.spirit;
        break;
    default:
        fprintf(stderr, "Dans required_nb_monsters, l'argument monstre doit être un ZOMBIE|GHOST|VAMPIRE|SPIRIT\n");
        exit(EXIT_FAILURE);
        break;
    }

    return nombreDeMonstres;
}

bool is_game_over(cgame g)
{
    if(!g)
    {
        fprintf(stderr, "Dans is_game_over: pointeur null\n");
        exit(EXIT_FAILURE);
    }
    /* le jeu est terminé si et seulement si:
     * le nombre de monstres pouvant être vu (current_nb_seen(...)) et le nombre
     * de monstre ciblé pour être vu (required_nb_seen(...)) sont identique selon chaque position
     * de tous les côtés
     */
    //verification pour les cotés N et S qui dependent de la largeur de la grille
    for(int i=0; i<g->width; i++)
    {
        if(current_nb_seen(g,N,i)!=required_nb_seen(g,N,i) || current_nb_seen(g,S,i)!=required_nb_seen(g,S,i))
            return false;
    }
    //verification pour les cotés E et W qui dependent de la hauteur de la grille
    for(int i=0; i<g->height; i++)
    {
        if(current_nb_seen(g,E,i)!=required_nb_seen(g,E,i) || current_nb_seen(g,W,i)!=required_nb_seen(g,W,i))
            return false;
    }

    return true;
}

void restart_game(game g)
{
    if(!g)
    {
        fprintf(stderr,"Dans restart_game: pointeur null\n");
        exit(EXIT_FAILURE);
    }
    //parcourir la grille et mettre tous les contents (de type monstres) à EMPTY
    content contenu;
    for(int i=0;i<g->width;i++)
    {
        for(int j=0;j<g->height;j++)
        {
            contenu = get_content(g,i,j);
            //seul les content monstres sont remplacer par EMPTY
            if(contenu == GHOST || contenu==VAMPIRE || contenu==ZOMBIE || contenu==SPIRIT)
                g->squares[i][j]=EMPTY;
        }
    }
    // reinitialiser le nombre de monstres courants
    g->nb_current_monster.vampire=NOMBRE_MONSTRE_COURANT_INITIAL;
    g->nb_current_monster.ghost=NOMBRE_MONSTRE_COURANT_INITIAL;
    g->nb_current_monster.zombie=NOMBRE_MONSTRE_COURANT_INITIAL;
    g->nb_current_monster.spirit=NOMBRE_MONSTRE_COURANT_INITIAL;
}

bool add_monster(game game, content monster, int col, int line)
{
    if(!game)
    {
        fprintf(stdout, "Le pointeur passé a add_monster est null\n");
        return false;
    }
    // contrôle de la validité des coordonnées
    if(col<INDICE_MINIMUM || col>game->width-1 || line<INDICE_MINIMUM || line>game->height-1)
    {
        fprintf(stdout,"Dans add_monster, les paramètre col et line sont invalides\n");
        return false;
    }
    /*premier contrôle necessaire, pour eviter de decrementer le nombre de monstre courant
     * dans le switch(game->squares[col][line] alors que le traitement sera ignoré si le paramètre
     * monstre est invalide plus bas dans switch(monster)
     */
    if(monster!=GHOST && monster!=VAMPIRE && monster!=ZOMBIE && monster!=EMPTY && monster!=SPIRIT)
    {
        fprintf(stdout, "Dans add_monster, l'argument monstre doit être un ZOMBIE|GHOST|VAMPIRE\n");
        return false;
    }
    //si à la position indiquée se trouve déjà le même content, le traitement est ignoré
    if(monster == game->squares[col][line])
        return true;
    else{ // sinon on check le contenu de la case cible, pour mettre à jour le nombre de monstres courants
        switch (game->squares[col][line]) {
        case MIRROR:    // pas de traitement pour tout type de mirroir
        case ANTIMIRROR:
        case VMIRROR:
        case HMIRROR:
            fprintf(stderr,"Dans add_monster: vous ne pouvez pas ajouter un monstre sur un mirroir\n");
            return false;
            break;
            /* pour les 4 type de monstres, on doit diminuer le nombre de monstres courants à 1
             *puisque le monstre situé à cette case sera remplacé par le nouveau qui n'est pas le même
             */
        case ZOMBIE:
            game->nb_current_monster.zombie -= 1;
            break;
        case VAMPIRE:
            game->nb_current_monster.vampire -= 1;
            break;
        case GHOST:
            game->nb_current_monster.ghost -= 1;
            break;
        case SPIRIT:
            game->nb_current_monster.spirit -= 1;
            break;
        default:
            break;
        }
    }
    // on met le nombre de monstre courants à jour en l'incrementant selon le type de monstre
    switch (monster) {
    case ZOMBIE:
        game->nb_current_monster.zombie += 1;
        break;
    case VAMPIRE:
        game->nb_current_monster.vampire += 1;
        break;
    case GHOST:
        game->nb_current_monster.ghost += 1;
        break;
    case SPIRIT:
        game->nb_current_monster.spirit += 1;
        break;
    case EMPTY:
        break;
    default:
        fprintf(stdout,"Dans add_monster, le type de monstre est invalide\n");
        return false;
        break;
    }

    // tout est ok on place le monstre dans la grille
    game->squares[col][line] = monster;

    return true;
}

int current_nb_seen(cgame game, direction side, int pos)
{
    if(!game)
    {
        fprintf(stderr, "Dans current_nb_seen: pointeur null\n");
        exit(EXIT_FAILURE);
    }
    int ligne = 0, colonne = 0;
    int nombreDeMonstres = 0;
    bool mirroirDetecte = false;
    bool continuerParcour = true;
    directionRayon directionCourante; // c'est le sens vers lequel le rayon se dirige à un moment x

    //verifier si la position est valide
    if(pos<INDICE_MINIMUM){
        fprintf(stderr,"Dans current_nb_seen: le parametre position doit être positif\n");
        exit(EXIT_FAILURE);
    }
    //on controle si la postion est valide et on determine la position de depart
    switch(side)
    {
    case N: // N et S, en fonction de la largeur
    case S:
        // controle de la position
        if(pos>=game->width){
            fprintf(stderr,"Dans current_nb_seen: le parametre position doit être inferieur à %d\n", game->width);
            exit(EXIT_FAILURE);
        }
        colonne = pos;
        // côté Nord, le parcours commence à la derniere ligne (hauteur-1)
        if(side==N){
            ligne = game->height-1;
            directionCourante = BAS;    // direction de haut en BAS
        }
        else{ // côté sud, le parcours commence à la première ligne (hauteur==0)
            ligne = 0;
            directionCourante = HAUT;   // direction de bas en HAUT
        }
        break;
    case E: // E et W, en fonction de la hauteur
    case W:
        // controle de la position
        if(pos>=game->height){
            fprintf(stderr,"Dans current_nb_seen: le parametre position doit être inferieur à %d\n", game->height);
            exit(EXIT_FAILURE);
        }
        ligne = pos;
        //côté West, le parcours commence le parcours commence à la premiere colonne
        if(side==W) {
            colonne = 0;
            directionCourante = DROITE; // direction de la GAUCHE vers la droite

        }
        else{// côté west, le parcours commence à la derniere colonne (colonne-1)
            colonne = game->width-1;
            directionCourante = GAUCHE; // direction de la droite vers la gauche
        }
        break;
    }
    do{
        content contenu = get_content(game,colonne,ligne);
        actualiserNombreDeMonstres(contenu, directionCourante, &mirroirDetecte, &nombreDeMonstres);
        determinerCaseSuivante(contenu,&directionCourante,&ligne,&colonne);
        //contrôle de debordement
        if(colonne<0 || colonne>=game->width || ligne<0 || ligne>=game->height)
            continuerParcour = false;
    }while(continuerParcour);    //tant qu'il n'y pas de debordement on calcul le nombre de monstres pouvant être vus

    return nombreDeMonstres;
}
int current_nb_monsters(cgame game, content monster)
{
    if(!game)
    {
        fprintf(stderr,"Dans current_nb_monster: pointeur null\n");
        exit(EXIT_FAILURE);
    }
    // retourner le nombre de monstres courants si monster est valide, sinon ignorer le traitement
    switch (monster)
    {
    case VAMPIRE:
        return game->nb_current_monster.vampire;
        break;
    case ZOMBIE:
        return game->nb_current_monster.zombie;
        break;
    case GHOST:
        return game->nb_current_monster.ghost;
        break;
    case SPIRIT:
        return game->nb_current_monster.spirit;
        break;
    default:
        fprintf(stderr, "Dans current_nb_monsters: le parma monster doit être ZOMBIE|GHOST|VAMPIRE|SPIRIT\n");
        exit(EXIT_FAILURE);
        break;
    }
}

game setup_new_game(int *labels[NB_DIR],content *board, int required_nb_ghosts, int required_nb_vampires, int required_nb_zombies)
{
    // utiliser setup_new_game_ext en mettant le nombre de monstres pour SPIRIT à 0
    game monJeu = setup_new_game_ext(TAILLE_GRILLE_FIXE,TAILLE_GRILLE_FIXE,labels,board,
                                     required_nb_ghosts,required_nb_vampires,required_nb_zombies,0);
    return monJeu;
}

game new_game_ext(int width, int height)
{
    /*on ne traite pas des valeurs négatives pour les dimensions de la grille */
    if(width<=INDICE_MINIMUM || height<=INDICE_MINIMUM)
    {
        fprintf(stderr,"La largeur et la hauteur doit être strictement supérieur à 0\n");
        exit(EXIT_FAILURE);
    }
    //allocation de la structure principale du jeu
    game monJeu = (game)malloc(sizeof(struct game_s));
    if(!monJeu){
        erreurAllocation();
    }
    monJeu->height = height;
    monJeu->width = width;
    //allocation d'un tableau à deux dimensions pour les contents
    monJeu->squares = (content**)malloc(monJeu->width * sizeof(content*));
    if(!monJeu->squares){
        erreurAllocation();
    }
    //alloctions des sous tableaux de type content content
    for(int i=0;i < monJeu->width; i++)
    {
        monJeu->squares[i] = (content*)malloc(monJeu->height * sizeof(content));
        if(monJeu->squares[i] == NULL){
            erreurAllocation();
        }
    }
    //allocation d'un tableau à deux dimensions pour les labels de taille NB_DIR
    monJeu->labels = (int**) malloc(NB_DIR * sizeof(int*));
    if(!monJeu->labels) {
        erreurAllocation();
    }
    /*allocation des sous tableaux de type int pour les labels
     * pour chacun des côtés N et S, on alloue un tableau dont la taille est égale à la largeur
     * pour chacun des côtés E et W, on alloue un tableau dont la taille est égale à la hauteur
     */
    for(int i=0; i<NB_DIR; i++)
    {
        switch(i)
        {
        case N:
        case S:
            monJeu->labels[i] = (int*)malloc(monJeu->width * sizeof(int));
            if(!monJeu->labels[i])
                erreurAllocation();
            break;
        case E:
        case W:
            monJeu->labels[i] = (int*)malloc(monJeu->height * sizeof(int));
            if(!monJeu->labels[i])
                erreurAllocation();
            break;
        }
    }
    //Mise de tous les contents à empty
    for(int i=0; i<monJeu->width; i++)
    {
        for(int j=0; j<monJeu->height; j++)
        {
            monJeu->squares[i][j] = EMPTY;
        }
    }
    //Initialiser le nombre de monstre requis
    monJeu->nb_monstre_requis.ghost = NOMBRE_MONSTRE_REQUIS_INITIAL;
    monJeu->nb_monstre_requis.vampire = NOMBRE_MONSTRE_REQUIS_INITIAL;
    monJeu->nb_monstre_requis.zombie = NOMBRE_MONSTRE_REQUIS_INITIAL;
    monJeu->nb_monstre_requis.spirit = NOMBRE_MONSTRE_REQUIS_INITIAL;
    /* mise de tous les labels à 0*/
    //on parcours les labels N et S ensembles comme ils sont tous en fonction de la largeur
    for(int j=0; j<monJeu->width; j++)
    {
        monJeu->labels[N][j]=0;
        monJeu->labels[S][j]=0;
    }
    // on parcours les labels E et W ensembles aussi comme ils sont en fonction de la hauteur
    for(int j=0; j<monJeu->height; j++)
    {
        monJeu->labels[E][j]=0;
        monJeu->labels[W][j]=0;
    }
    //initialisation du nombres de monstres courants
    monJeu->nb_current_monster.ghost = NOMBRE_MONSTRE_COURANT_INITIAL;
    monJeu->nb_current_monster.vampire = NOMBRE_MONSTRE_COURANT_INITIAL;
    monJeu->nb_current_monster.zombie = NOMBRE_MONSTRE_COURANT_INITIAL;
    monJeu->nb_current_monster.spirit = NOMBRE_MONSTRE_COURANT_INITIAL;

    return monJeu;
}

game setup_new_game_ext(int width, int height, int *labels[NB_DIR], content * board,
                        int required_nb_ghosts,  int required_nb_vampires,
                        int required_nb_zombies, int required_nb_spirits)
{
    //contrôle des pointeurs
    if(!labels || !board)
    {
        fprintf(stderr,"Dans setup_new_game_ext: pointeur null\n");
        exit(EXIT_FAILURE);
    }
    //contrôle des paramètes de type int
    if(width<=0 || height<=0 || required_nb_ghosts<0 || required_nb_spirits<0 ||
            required_nb_vampires<0 || required_nb_zombies<0)
    {
        fprintf(stderr,"Erreur dans setup_new_game_ext: les paramètres entiers doivent être strictement supérieur à 0\n");
        exit(EXIT_FAILURE);
    }
    // creer le jeu avec new_game_ext en précisant les dimensions
    game g = new_game_ext(width, height);
    assert(g);
    // modifier les informations du jeu crée selon les paramètres reçu
    // pour le nombre de monstres requis
    g->nb_monstre_requis.vampire = required_nb_vampires;
    g->nb_monstre_requis.zombie = required_nb_zombies;
    g->nb_monstre_requis.ghost = required_nb_ghosts;
    g->nb_monstre_requis.spirit = required_nb_spirits;
    //pour le tableau de contents
    int k=0; // utilisé comme indice continu pour le tableau board
    for(int i=0; i<height; i++)
    {
        for(int j=0;j<width; j++)
        {
            g->squares[j][i] = board[k];
            k++;
        }
        printf("\n");
    }

    /*pour les labels nord et sud en fonction de la largeur*/
    for(int i=0; i<width; i++)
    {
        g->labels[N][i] = labels[N][i];
        g->labels[S][i] = labels[S][i];
    }
    /*pour les labels est et ouest en fonction de la hauteur */
    for(int i=0; i<height; i++)
    {
        g->labels[E][i] = labels[E][i];
        g->labels[W][i] = labels[W][i];
    }

    return g;
}
int game_height(cgame game){
    if(game==NULL)
    {
        fprintf(stderr,"Dans game_height: null pointer\n");
        exit(EXIT_FAILURE);
    }
    return game->height;

}

int game_width(cgame game){
    if(game==NULL)
    {
        fprintf(stderr,"Dans game_width: null pointeur\n");
        exit(EXIT_FAILURE);
    }
    return game->width;
}

void add_mirror_ext(game game, content mirror_type, int col, int line)
{
    if(!game)
    {
        fprintf(stderr,"pointeur null dans  add_mirror\n");
        exit(EXIT_FAILURE);
    }
    // verification de la positon
    if(col<INDICE_MINIMUM || col>game->width-1 || line<INDICE_MINIMUM || line>game->height-1)
    {
        fprintf(stderr,"Dans add_mirror_ext: col et line sont invalides\n");
        return;
    }
    //on verifie s'il y'avais un monstre à la postion où il veut placer le mirroir
    //afin d'actualiser le nombre de monstres courants en le decrementant de 1
    switch (game->squares[col][line]) {
    case VAMPIRE:
        game->nb_current_monster.vampire-=1;
        break;
    case GHOST:
        game->nb_current_monster.ghost-=1;
        break;
    case ZOMBIE:
        game->nb_current_monster.zombie-=1;
        break;
    case SPIRIT:
        game->nb_current_monster.spirit-=1;
        break;
    default:
        break;
    }
    // placer le mirroir s'il est valide ou ignorer le traitement
    switch(mirror_type)
    {
    case MIRROR:
        game->squares[col][line] = MIRROR;
        break;
    case ANTIMIRROR:
        game->squares[col][line] = ANTIMIRROR;
        break;
    case VMIRROR:
        game->squares[col][line] = VMIRROR;
        break;
    case HMIRROR:
        game->squares[col][line] = HMIRROR;
        break;
    default:
        fprintf(stderr,"Dans add_mirror_ext: type de mirroir invalide\n");
        return;
        break;
    }
}

void actualiserNombreDeMonstres(content contenu,directionRayon directionCourante,bool *mirroirDetecte,int* nombreDeMonstres)
{
    switch(contenu){
    case MIRROR:     // pour mirror et antimirror, indiquer juste qu'un mirroir a été detecté
    case ANTIMIRROR:
        *mirroirDetecte = true;
        break;
    case VMIRROR:
        /*Dans ce cas, le mirroir n'est detectée que si la direction n'est
         * pas dans le meme sens que le mirroir*/
        if(directionCourante!=HAUT && directionCourante!=BAS){
            *mirroirDetecte = true;
        }
        break;
    case HMIRROR:
        /*meme commentaire que sur VMIRROR */
        if(directionCourante!=GAUCHE && directionCourante!=DROITE){
            *mirroirDetecte = true;
        }
        break;
    case ZOMBIE:
        (*nombreDeMonstres)++;  // un zombie est toujous vu
        break;
    case VAMPIRE:
        if(!(*mirroirDetecte)){ // les vampires ne sont vu que si un mirroir n'a pas été detecté
            (*nombreDeMonstres)++;
        }
        break;
    case GHOST:
        if(*mirroirDetecte){    // les ghosts ne sont vu que un mirroir a été dectecté
            (*nombreDeMonstres)++;
        }
        break;
    default:
        break;
    }
}
void determinerCaseSuivante(content contenu, directionRayon *directionCourante, int* ligne, int* colonne)
{
    switch(contenu)
    {
    case VAMPIRE:
    case ZOMBIE:
    case GHOST:
    case SPIRIT:
    case EMPTY:
        /*si on est positionné sur un monstre, on se deplace d'une case
         * en gardant la même direction courante
         */
        switch(*directionCourante)
        {
        case BAS:
            (*ligne)--;     // decsendre d'une case
            break;
        case HAUT:
            (*ligne)++;     // monter d'une case
            break;
        case GAUCHE:
            (*colonne)--;   // aller à gauche d'une case
            break;
        case DROITE:
            (*colonne)++;   // aller à droite d'une case
            break;
        }
        break;
        /* pour le cas des mirroir:
         * on change de direction selon le type de mirroirs et la direction courante
         * on actualise directionCourante
         */
    case MIRROR:
        switch(*directionCourante)
        {
        case BAS:
            (*colonne)--;   // prendre la case à gauche
            *directionCourante = GAUCHE;
            break;
        case HAUT:
            (*colonne)++;   // prendre la case à droite
            *directionCourante = DROITE;
            break;
        case GAUCHE:
            (*ligne)--;     // prendre la case au dessou
            *directionCourante = BAS;
            break;
        case DROITE:
            (*ligne)++;     // prendre la case au dessu
            *directionCourante = HAUT;
            break;
        }
        break;
    case ANTIMIRROR:
        switch(*directionCourante)
        {
        case BAS:
            (*colonne)++;   // prendre la case à droite
            *directionCourante = DROITE;
            break;
        case HAUT:
            (*colonne)--;   // prendre la case à gauche
            *directionCourante = GAUCHE;
            break;
        case GAUCHE:
            (*ligne)++;     // prendre la case au dessus
            *directionCourante = HAUT;
            break;
        case DROITE:
            (*ligne)--;     // prendre la case au dessous
            *directionCourante = BAS;
            break;
        }
        break;
    case HMIRROR:
        switch(*directionCourante)
        {
        case BAS:
            (*ligne)++;     // prendre la case au dessus
            *directionCourante = HAUT;
            break;
        case HAUT:
            (*ligne)--;     // prendre la case au dessous
            *directionCourante = BAS;
            break;
        case GAUCHE:
            (*colonne)--;   // prendre la case à gauche
            *directionCourante = GAUCHE;
            break;
        case DROITE:
            (*colonne)++;   // prendre la case  à droite
            *directionCourante = DROITE;
            break;
        }
        break;
    case VMIRROR:
        switch(*directionCourante)
        {
        case BAS:
            (*ligne)--;      // prendre la case au dessou
            *directionCourante = BAS;
            break;
        case HAUT:
            (*ligne)++;     // prendre la case au dessu
            *directionCourante = HAUT;
            break;
        case GAUCHE:
            (*colonne)++;   // prendre la case à droite
            *directionCourante = DROITE;
            break;
        case DROITE:
            (*colonne)--;   // prendre la case à gauche
            *directionCourante = GAUCHE;
            break;
        }
        break;
    default:
        break;
    }
}

void erreurAllocation(){
    fprintf(stderr,"allocation impossible\n");
    exit(EXIT_FAILURE);
}
