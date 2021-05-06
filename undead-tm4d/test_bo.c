#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "game_io.h"
#include "game.h"


bool test_get_content(){
    bool res=true;
    game g = new_game_ext(4, 4);
    assert(g);
    //test sur le retours de la fonction
    add_monster(g, GHOST, 0, 1);
    add_monster(g, ZOMBIE, 1, 1);
    add_monster(g, VAMPIRE, 2, 3);
    add_monster(g, SPIRIT, 3, 3);
    if(get_content(g, 0, 1)!=GHOST || get_content(g, 1, 1)!=ZOMBIE || get_content(g, 2, 3)!=VAMPIRE || get_content(g, 3, 3)!=SPIRIT){
        fprintf(stderr,("Dans test_get_content: get_content ne retourne pas les monstres correspondant\n"));
        res=false;
    }
    delete_game(g);
    return res;
}

bool test_set_required_nb_monsters()
{
    bool res = true;
    game g = new_game();
    assert(g);
    //si le retour de la fonction est differente de la valeur placée, pas ok...
    set_required_nb_monsters(g, ZOMBIE, 5);
    set_required_nb_monsters(g, VAMPIRE, 2);
    set_required_nb_monsters(g, GHOST, 2);
    set_required_nb_monsters(g, SPIRIT, 5);
    if(required_nb_monsters(g,ZOMBIE)!=5 || required_nb_monsters(g, VAMPIRE)!=2 || required_nb_monsters(g, GHOST)!=2 || required_nb_monsters(g, SPIRIT)!=5){
        fprintf(stderr,"\nErreur dans set_required_nb_monsters: la valeur de retour de la fonction est incorrecte\n\n");
        res = false;
    }
    delete_game(g);
    return res;
}

int main(void){
    bool result = true;
    result = test_get_content() && result;
    result = test_set_required_nb_monsters() && result;;
    if(result){
        printf("Tests booba successfull\n");
        return EXIT_SUCCESS;
    }
    else{
        printf("Tests booba failed\n");
        return EXIT_FAILURE;
    }
}

