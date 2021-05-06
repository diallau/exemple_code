#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "game_io.h"
#include "game.h"

void gagner(game g);
void ajouterMirroirs(game monJeu);
void perdu(game g);
void nullPointeur(char* message);
bool test_new_game_ext();
bool test_add_mirror_ext();
bool test_game_width();
bool test_game_height();
bool test_setup_new_game_ext();

bool test_add_monster(){
    bool res = true;
    game g = new_game_ext(4,4);
    assert(g);
    // tester l'ajout d'un monstre sur un type de mirroir
    add_mirror(g, 0, 0, 0);
    add_mirror(g, 1, 2, 1);
    add_mirror_ext(g,HMIRROR,3,0);
    add_mirror_ext(g,VMIRROR,3,1);
    if(add_monster(g,VAMPIRE, 0, 0) || add_monster(g, GHOST, 2, 1) || add_monster(g,SPIRIT, 3, 0) || add_monster(g,ZOMBIE,3,1)){
        fprintf(stderr, "\nErreur dans test_add_monster: impossible d'ajouter un monstre sur un MIRROR ou un ANTIMIRROR\n\n");
        res = false;
    }
    //test sur le retour de la fonction sur une case valide
    if(!add_monster(g, ZOMBIE, 0, 1) || !add_monster(g, VAMPIRE, 0, 2) || !add_monster(g, GHOST, 0,3) || !add_monster(g, SPIRIT, 1, 0))
    {
        fprintf(stderr, "\nErreur dans add_monstre: add_monstre n'arrive pas a ajouter un monstre sur une case valide\n\n");
        res = false;
    }
    //verification du nombre de mosntres courants
    if(current_nb_monsters(g,VAMPIRE)!=1 || current_nb_monsters(g,ZOMBIE)!=1 || current_nb_monsters(g,GHOST)!=1 || current_nb_monsters(g,SPIRIT)!=1)
    {
        fprintf(stderr,"\nErreur dans test_add_monster: le nombre de monstre courant n'est pas correcte\n");
        res = false;
    }
    //verification de  la suppression d'un monstre evec empty
    add_monster(g,EMPTY,0,1);
    add_monster(g,EMPTY,0,2);
    add_monster(g,EMPTY,0,3);
    add_monster(g,EMPTY,1,0);
    if(get_content(g,0,1)==ZOMBIE || get_content(g,0,2)==VAMPIRE || get_content(g,0,3)==GHOST || get_content(g,1,0)==SPIRIT){
        fprintf(stderr,"\nDans test_add_monster: la suppression de monstres avec EMPTY ne marche pas\n");
        res = false;
    }
    if(current_nb_monsters(g,VAMPIRE)!=0 || current_nb_monsters(g,ZOMBIE)!=0 || current_nb_monsters(g,GHOST)!=0 || current_nb_monsters(g,SPIRIT)!=0)
    {
        fprintf(stderr,"\nErreur dans test_add_monster: le nombre de monstre courant n'est pas correcte après la suppression d'un monstre\n");
        res = false;
    }
    delete_game(g);
    return res;
}
bool test_is_game_over(){
    bool res = true;
    game game1 = new_game();
    game game2 = new_game();
    assert(game1);
    assert(game2);
    ajouterMirroirs(game1); //je place des mirroirs dans la grille
    gagner(game1);     // rempli la grille avec la solution correspondante (les monstre)
    /* si la fonction retourne false dans ce cas, pas ok */
    if(!is_game_over(game1)){
        fprintf(stderr, "\nErreur dans is_game_over:La fonction doit renvoyer true si la solution a été trouvée\n\n");
        res = false;
    }
    //si elle retourne true alors que la solution n'a pas été trouvée, pas ok...
    ajouterMirroirs(game2);
    perdu(game2);
    if(is_game_over(game2)){
        fprintf(stderr,"\nErreur dans is_game_over: la fonction ne doit pas renvoyer true si la solution n'a pas été trouvée\n\n");
    }
    delete_game(game1);
    delete_game(game2);
    return res;
}
//---------------------------------------------------------------------------
bool test_required_nb_monster(){
    bool res = true;
    game g = new_game();
    assert(g);
    //si le nombre de monstres est != 0, pas ok
    if(required_nb_monsters(g, ZOMBIE)!=0 || required_nb_monsters(g, GHOST)!=0 || required_nb_monsters(g, VAMPIRE)!=0 || required_nb_monsters(g, SPIRIT)!=0){
        fprintf(stderr, "\nErreur dans required_nb_monsters: Le nombre retourné ne peut pas être négatif\n");
        res = false;
    }
    //si le retour est different des valeurs placées, pas ok
    set_required_nb_monsters(g, ZOMBIE, 5);
    set_required_nb_monsters(g, VAMPIRE, 2);
    set_required_nb_monsters(g, GHOST, 2);
    set_required_nb_monsters(g, SPIRIT, 3);
    if(required_nb_monsters(g,ZOMBIE)!=5 || required_nb_monsters(g, VAMPIRE)!=2 || required_nb_monsters(g, GHOST)!=2 || required_nb_monsters(g, SPIRIT)!=3){
        fprintf(stderr,"\nErreur dans required_nb_monsters: la valeur de retour de la fonction est incorrecte\n\n");
        res = false;
    }
    delete_game(g);
    return res;
}

bool test_current_nb_seen(){
    bool res = true;
    int tailleGrille=0; // une seule dimension puisque je cree le jeu avec new_game();
    game g = new_game();
    assert(g);
    /*
     * si après la solution trouvée, la fonction retourne
     * une valeur differente de celle de required_nb_seen, pas ok..
     */
    ajouterMirroirs(g);  // je place des mirroir dans la gille
    gagner(g);
    tailleGrille = game_width(g);
    for(int i=0; i<tailleGrille; i++){
        if((current_nb_seen(g, S, i) != required_nb_seen(g, S, i))|| (current_nb_seen(g, N, i) != required_nb_seen(g, N, i))
                || (current_nb_seen(g, E, i) != required_nb_seen(g, E, i))||(current_nb_seen(g, W, i) != required_nb_seen(g, W, i))){
            fprintf(stderr, "\nErreur dans test_current_nb_seen: current_nb_seen ne retourne pas la valeur normale\n\n");
            res = false;
            break;
        }
    }
    delete_game(g);
    return res;
}

bool test_current_nb_monsters(){
    /*
     * ce test est effectué en se basant sur la solution d'une grille 4 x 4
     */
    bool res = true;
    game g = new_game_ext(4,4);
    assert(g);
    //si après la creation d'un jeu, la fonction retourne un nombre different de 0, pas ok...
    if(current_nb_monsters(g, ZOMBIE) != 0 || current_nb_monsters(g, GHOST) != 0 || current_nb_monsters(g, VAMPIRE) != 0 || current_nb_monsters(g, SPIRIT)!=0){
        fprintf(stderr, "\nErreur1 dans current_nb_monsters: la valeur de retour est incorrecte\n\n");
        res = false;
    }
    //après avoir gagné, si le Z!=5 V!=2 G!=2, pas ok...
    ajouterMirroirs(g);
    gagner(g);
    if(current_nb_monsters(g, ZOMBIE) != required_nb_monsters(g, ZOMBIE) || current_nb_monsters(g, GHOST)!= required_nb_monsters(g, GHOST)
            || current_nb_monsters(g, VAMPIRE)!=required_nb_monsters(g, VAMPIRE) || current_nb_monsters(g, SPIRIT)!=required_nb_monsters(g, SPIRIT)){
        fprintf(stderr, "\nErreur2 dans current_nb_monsters: La valeur de retour de la fonction est incorrecte\n\n");
        res = false;
    }
    delete_game(g);
    g = new_game();
    add_monster(g,SPIRIT,0,0);
    add_monster(g,ZOMBIE,0,1);
    add_monster(g,SPIRIT,0,0);
    add_monster(g,ZOMBIE,0,1);
    if(current_nb_monsters(g,SPIRIT)!=1 || current_nb_monsters(g,ZOMBIE)!=1){
        fprintf(stderr,"\n Erreur3 dans test_current_nb_seen: le nombre de monstres courants n'est pas correcte\n");
        res = false;
    }
    add_monster(g,VAMPIRE,0,1);
    add_monster(g,GHOST,0,0);
    if(current_nb_monsters(g,SPIRIT)!=0 || current_nb_monsters(g,ZOMBIE)!=0)
    {
        fprintf(stderr,"\nErreur4 dans test_current_nb_seen: nombre de monstres incorrectes\n");
        res = false;
    }
    if(current_nb_monsters(g,GHOST)!=1 || current_nb_monsters(g,VAMPIRE)!=1)
    {
        fprintf(stderr,"\nErreur dans test_current_nb_seen: nombre de monstres incorrectes\n");
        res = false;
    }
    add_monster(g,SPIRIT,2,0);
    add_mirror_ext(g,VMIRROR,2,0);
    if(current_nb_monsters(g,SPIRIT)!=0){
        fprintf(stderr,"Erreur5, dans test_current_nb_seen: nombre de monstres incorrectes\n");
        res = false;
    }
    delete_game(g);
    return res;
}
bool test_restart_game(){
    bool res=true;
    content contenu = EMPTY;
    game g = new_game_ext(4,4);
    int largeur = game_width(g);
    int hauteur = game_height(g);
    assert(g);
    //si il y'a au moins un monstre après l'appel de restart_game, pas ok ...
    gagner(g); // je place les monstre donnant la solution
    restart_game(g);
    for(int colonne=0; colonne<largeur; colonne++){
        for(int ligne=0; ligne<hauteur; ligne++){
            contenu = get_content(g, colonne, ligne);
            if(contenu == VAMPIRE || contenu == GHOST || contenu == ZOMBIE || contenu == SPIRIT){
                fprintf(stderr, "\nErreur dans  restart_game: La fonction ne supprime pas les monstres pour le redemarrage du jeu \n\n");
                res = false;
            }
        }
    }
    delete_game(g);
    return res;
}
bool test_setup_new_game(){
    /*
     * ce teste est basée sur une grille 4 * 4
     */
    bool res = true;
    int tabN[]={0,3,3,0};
    int tabE[]={0,0,3,2};
    int tabW[]={0,2,3,3};
    int tabS[]={0,3,2,3};
    int *labels[]={tabN, tabS, tabE, tabW};
    int valN[4]={0};
    int valS[4]={0};
    int valE[4]={0};
    int valW[4]={0};
    content tab[16];
    game g = setup_new_game(labels, tab,2, 2, 5);
    //pour le nombre de monstres requis
    if((required_nb_monsters(g,ZOMBIE)!=5) || (required_nb_monsters(g,VAMPIRE)!=2) || (required_nb_monsters(g,GHOST)!=2)){
        fprintf(stderr, "Erreur dans setup_new_game: Le nombre de monstres requis est erroné\n\n");
        res = false;
    }
    //pour les labels
    for(int i=0;i<4;i++){
        valN[i] = required_nb_seen(g, N, i);
    }
    for(int i=0;i<4;i++){
        valS[i] = required_nb_seen(g, S, i);
    }
    for(int i=0;i<4;i++){
        valE[i] = required_nb_seen(g, E, i);
    }
    for(int i=0;i<4;i++){
        valW[i] = required_nb_seen(g, W, i);
    }
    if(valN[0]!=0 || valN[1]!=3 || valN[2]!=3 || valN[3]!=0 || valS[0]!=0 || valS[1]!=3 || valS[2]!=2 || valS[3]!=3
            || valE[0]!=0 || valE[1]!=0 || valE[2]!=3 || valE[3]!=2 || valW[0]!=0 || valW[1]!=2 || valW[2]!=3 || valW[3]!=3){
        fprintf(stderr, "\nErreur dans setup_new_game: les labels n'ont pas été construits correctement\n\n");
        res = false;
    }
    delete_game(g);
    return res;
}
int main(int argc,char *argv[]){
    bool result = true;
    result = test_add_monster() && result;
    result = test_is_game_over() && result;
    result = test_required_nb_monster() && result;
    result = test_current_nb_seen() && result;
    result = test_restart_game() && result;
    result = test_current_nb_monsters() && result;
    result = test_setup_new_game() && result;
    result = test_new_game_ext() && result;
    result = test_add_mirror_ext()&& result;
    result = test_game_width() && result;
    result = test_game_height() && result;
    result = test_setup_new_game_ext() && result;
    if(result){
        printf("Tests elhajd successful\n");
        return EXIT_SUCCESS;
    }
    else{
        printf("tests elhadj failed\n");
        return EXIT_FAILURE;
    }
}
void gagner(game g){
    add_monster(g, ZOMBIE,1 ,2);
    add_monster(g, ZOMBIE,2 ,2);
    add_monster(g, ZOMBIE,3 ,2);
    add_monster(g, ZOMBIE,0 ,1);
    add_monster(g, GHOST,1 ,1);
    add_monster(g, ZOMBIE,1 ,0);
    add_monster(g, GHOST,2 ,0);
    add_monster(g, VAMPIRE,1 ,3);
    add_monster(g, VAMPIRE,2 ,3);
}
void perdu(game g){
    add_monster(g, ZOMBIE,1 ,2);
    add_monster(g, ZOMBIE,2 ,2);
    add_monster(g, ZOMBIE,3 ,2);
    add_monster(g, ZOMBIE,0 ,1);
    add_monster(g, GHOST,1 ,1);
    add_monster(g, ZOMBIE,1 ,0);
    add_monster(g, GHOST,2 ,0);
    add_monster(g, VAMPIRE,1 ,3);
    add_monster(g, ZOMBIE,2 ,3);
}
void ajouterMirroirs(game monJeu){
    add_mirror(monJeu,1,0,3);
    add_mirror(monJeu,0,3,3);
    add_mirror(monJeu,1,0,2);
    add_mirror(monJeu,1,2,1);
    add_mirror(monJeu,0,3,1);
    add_mirror(monJeu,1,0,0);
    add_mirror(monJeu,1,3,0);

    set_required_nb_monsters(monJeu,ZOMBIE,5);
    set_required_nb_monsters(monJeu,VAMPIRE,2);
    set_required_nb_monsters(monJeu,GHOST,2);
    //label nord
    set_required_nb_seen(monJeu,N,0,0);
    set_required_nb_seen(monJeu,N,1,3);
    set_required_nb_seen(monJeu,N,2,3);
    set_required_nb_seen(monJeu,N,3,0);
    //label ouest
    set_required_nb_seen(monJeu,W,0,0);
    set_required_nb_seen(monJeu,W,1,2);
    set_required_nb_seen(monJeu,W,2,3);
    set_required_nb_seen(monJeu,W,3,3);
    //label est
    set_required_nb_seen(monJeu,E,0,0);
    set_required_nb_seen(monJeu,E,1,0);
    set_required_nb_seen(monJeu,E,2,3);
    set_required_nb_seen(monJeu,E,3,2);

    //label sud
    set_required_nb_seen(monJeu,S,0,0);
    set_required_nb_seen(monJeu,S,1,3);
    set_required_nb_seen(monJeu,S,2,2);
    set_required_nb_seen(monJeu,S,3,3);
}
bool test_new_game_ext()
{
    bool correct = true;
    unsigned int largeur=4, hauteur=4;
    game g = NULL;
    g = new_game_ext(largeur,hauteur);
    if(g==NULL)
        nullPointeur("Dans test_new_game_ext: ");
    //verifaction sur ce que pretend faire la fonction
    //1-si toutes les cases sont vides c'est ok
    for(int x=0; x<largeur; x++){
        for(int y=0;y<hauteur;y++){
            content contenu = get_content(g, y, x);
            if(contenu != EMPTY)
            {
                fprintf(stderr, "\nErreur dans test_new_game: Les cases ne sont pas toutes à EMPTY \n\n");
                correct = false;
                goto end;
            }
            //*/
        }
    }
end:
    //2-si les required_nb_monsters sont à 0 ok
    if(required_nb_monsters(g,ZOMBIE)!=0 || required_nb_monsters(g,GHOST)!=0 || required_nb_monsters(g,VAMPIRE)!= 0 || required_nb_monsters(g, SPIRIT)!=0){
        fprintf(stderr, "\nErreur dans test new_game: Tous les required_nb_monsters ne sont pas à 0\n\n");
        correct = false;
    }
    //3-si tous les labels sont à 0 ok

    /*pour nord et sud en fonction de la largeur*/
    for(int i=0; i<largeur; i++){
        if(required_nb_seen(g, N, i)!=0 || required_nb_seen(g, S, i)!=0){
            fprintf(stderr, "\nErreur dans test new_game: Tous les labels ne sont pas à 0\n\n");
            correct = false;
            break;
        }
    }
    /*pour les labels est et ouest en fonction de la hauteur*/
    for(int i=0; i<hauteur; i++){
        if(required_nb_seen(g, E, i)!=0 || required_nb_seen(g, W, i)!=0){
            fprintf(stderr, "\nErreur dans test new_game: Tous les labels ne sont pas à 0\n\n");
            correct = false;
            break;
        }
    }

    delete_game(g);
    return correct;
}

void nullPointeur(char* message)
{
    fprintf(stderr,"%spointeur null\n",message);
    exit(EXIT_FAILURE);
}
bool test_add_mirror_ext(){
    bool res=true;
    game g = new_game_ext(4, 4);
    assert(g);
    add_mirror_ext(g, MIRROR, 1, 0);
    add_mirror_ext(g, ANTIMIRROR, 1, 1);
    add_mirror_ext(g, VMIRROR, 1, 2);
    add_mirror_ext(g, HMIRROR, 1, 3);
    if(get_content(g, 1, 0) != MIRROR || get_content(g, 1, 1) != ANTIMIRROR || get_content(g, 1, 2) != VMIRROR || get_content(g, 1, 3) != HMIRROR) {
        fprintf(stderr,"Erreur dans test_add_mirror_ext: la fonction ne place pas les mirroirs\n");
        res=false;
    }
    add_mirror_ext(g,ZOMBIE, 3,2);
    if(get_content(g,3,2)==ZOMBIE)
    {
        fprintf(stderr,"Dans test_add_mirror_ext: la fonction ne doit pas ajouter un autre content que des mirroirs\n");
        res = false;
    }
    delete_game(g);
    return res;
}
bool test_game_width()
{
    bool correct=true;
    game g = new_game_ext(4, 4);
    assert(g);
    if(game_width(g) != 4)
    {
        fprintf(stderr,"Erreur dans test_game_width: fonction incorrecte\n");
        correct = false;
    }
    delete_game(g);
    return correct;
}
bool test_game_height()
{
    bool correct=true;
    game g = new_game_ext(4, 4);
    assert(g);
    if(game_height(g) != 4)
    {
        fprintf(stderr,"Erreur dans test_game_height: fonction incorrecte\n");
        correct = false;
    }
    delete_game(g);
    return correct;
}
bool test_setup_new_game_ext()
{
    bool res = true;
    int largeur=4, hauteur=4;
    int tabN[]={0,3,3,0};
    int tabE[]={0,0,3,2};
    int tabW[]={0,2,3,3};
    int tabS[]={0,3,2,3};
    int *labels[4]={tabN, tabS, tabE, tabW};
    int valN[4]={0};
    int valS[4]={0};
    int valE[4]={0};
    int valW[4]={0};
    content tab[16]={VAMPIRE, SPIRIT, ZOMBIE, GHOST};
    game g = setup_new_game_ext(largeur, hauteur, labels, tab, 2, 2, 5, 5);
    //pour le nombre de monstres requis
    if((required_nb_monsters(g,ZOMBIE)!=5) || (required_nb_monsters(g,VAMPIRE)!=2)
            || (required_nb_monsters(g,GHOST)!=2) || (required_nb_monsters(g, SPIRIT)!=5)){
        fprintf(stderr, "Erreur dans setup_new_game_ext: Le nombre de monstres requis est erroné\n\n");
        res = false;
    }
    //verification du contenu de tab
    if(get_content(g, 0,0)!=VAMPIRE || get_content(g,1,0)!=SPIRIT || get_content(g,2,0)!=ZOMBIE || get_content(g,3,0)!=GHOST){
        fprintf(stderr,"Erreur dans test_setup_new_game_ext: le contenu de tab n'est pas correct\n");
        res = false;
    }
    /*
     * on recupere les labels pour les comparer avec celles qu'on à passer à la fonction
     * pour les labels nord et sud on utilise la largeur
     */
    for(int i=0;i<largeur;i++){
        valN[i] = required_nb_seen(g, N, i);
    }
    for(int i=0;i<largeur;i++){
        valS[i] = required_nb_seen(g, S, i);
    }
    /*pour les labels est et ouest on utilise la hauteur*/
    for(int i=0;i<hauteur;i++){
        valE[i] = required_nb_seen(g, E, i);
    }
    for(int i=0;i<hauteur;i++){
        valW[i] = required_nb_seen(g, W, i);
    }
    if(valN[0]!=0 || valN[1]!=3 || valN[2]!=3 || valN[3]!=0 || valS[0]!=0 || valS[1]!=3 || valS[2]!=2 || valS[3]!=3
            || valE[0]!=0 || valE[1]!=0 || valE[2]!=3 || valE[3]!=2 || valW[0]!=0 || valW[1]!=2 || valW[2]!=3 || valW[3]!=3){
        fprintf(stderr, "\nErreur dans setup_new_game_ext: les labels n'ont pas été construits correctement\n\n");
        res = false;
    }
    delete_game(g);
    return res;
}
