#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "game_io.h"
#include "game.h"

bool test_new_game(){
    bool res = true;
    game g = new_game();
    assert(g);
    int largeur = game_width(g);
    int hauteur = game_height(g);
    //si toutes les cases sont vides (EMPTY) c'est ok
    for(int x=0; x<largeur; x++){
        for(int y=0;y<hauteur;y++){
            if(get_content(g, y, x) != EMPTY)
            {
                fprintf(stderr, "\nDans test_new_game: Les cases ne sont toutes à EMPTY \n\n");
                res = false;
                goto end;   // j'arrête le parcourir dès qu'une case est incorrecte
            }
        }
    }
end:
    //2-si les required_nb_monsters sont à 0 ok
    if(required_nb_monsters(g,ZOMBIE)!=0 || required_nb_monsters(g,GHOST)!=0 || required_nb_monsters(g,VAMPIRE)!= 0 || required_nb_monsters(g,SPIRIT)!=0){
        fprintf(stderr, "\nErreur dans test new_game: Tous les required_nb_monsters ne sont pas à 0\n\n");
        res = false;
    }
    //3-si tous les labels sont à 0 ok
    for(int i=0; i<largeur; i++){
        if(required_nb_seen(g, N, i)!=0 || required_nb_seen(g, S, i)!=0 || required_nb_seen(g, E, i)!=0 || required_nb_seen(g, W, i)!=0){
            fprintf(stderr, "\nErreur dans test new_game: Tous les labels ne sont pas à 0\n\n");
            res = false;
            break;
        }
    }
    delete_game(g);
    return res;
}

bool test_add_mirror(){
    bool res=true;
    game g = new_game_ext(7,7);
    assert(g);
    add_mirror(g, 0, 1, 2);
    add_mirror(g, 1, 2, 3);
    if(get_content(g, 1, 2) != MIRROR || get_content(g, 2, 3)!=ANTIMIRROR) {
        fprintf(stderr,"Dans test_add_mirror: la fonction add_mirror ne place pas correctement les mirroirs\n");
        res=false;
    }
    delete_game(g);
    return res;
}

bool test_set_required_nb_seen(){
    bool res = true;
    game g = new_game();
    assert(g);
    int tailleGrille = game_width(g);
    /* on effectue des modifications sur les labels et on verifie si c'est ok */
    for(int i=0; i<tailleGrille; i++)
    {
        //modification
        set_required_nb_seen(g, N, i, i);
        set_required_nb_seen(g, S, i, i+1);
        set_required_nb_seen(g, E, i, i+2);
        set_required_nb_seen(g, W, i, i+3);
        //verification
        if(required_nb_seen(g, N, i)!=i || required_nb_seen(g, S, i)!=i+1 || required_nb_seen(g, E, i)!=i+2 || required_nb_seen(g, W, i)!=i+3)
        {
            fprintf(stderr, "\nErreur dans test_set_required_nb_seen: required_nb_seen ne retourne pas la valeur attendue\n\n");
            res = false;
            break;
        }
    }
    delete_game(g);
    return res;
}

int main(void){
    bool result = true;
    result = test_add_mirror() && result;
    result = test_new_game() && result;
    result = test_set_required_nb_seen() && result;
    if(result){
        printf("Tests david successfull\n");
        return EXIT_SUCCESS;
    }
    else{
        printf("Tests david failed\n");
        return EXIT_FAILURE;
    }
}

