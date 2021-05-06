#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include "game_io.h"
#include "game.h"

bool test_required_nb_seen()
{
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
            fprintf(stderr, "\nErreur dans test_required_nb_seen: required_nb_seen ne retourne pas la valeur attendue\n\n");
            res = false;
            break;
        }
    }
    delete_game(g);
    return res;
}

void test_delete_game()
{
    game g = new_game();
    delete_game(g);
}

bool test_copy_game(){
    bool res = true;
    game g = new_game();
    set_required_nb_monsters(g, ZOMBIE, 2);
    set_required_nb_seen(g, N, 0, 2);
    add_monster(g, VAMPIRE, 1, 2);
    add_mirror(g, 0, 1, 3);
    game g2 = copy_game(g);
    if((required_nb_monsters(g, ZOMBIE)) != (required_nb_monsters(g2, ZOMBIE)) || (required_nb_seen(g, N, 0)!= required_nb_seen(g2, N, 0))
            || get_content(g, 1, 2)!=VAMPIRE || get_content(g, 1, 3)!=MIRROR){
        fprintf(stderr, "\nErreur dans test_copy_game: le clonage du jeu n'a pas reussi\n\n");
        res = false;
    }
    delete_game(g);
    delete_game(g2);
    return res;
}

int main(void){
    bool result = true;
    result = test_required_nb_seen() && result;
    result = test_copy_game() && result;
    test_delete_game();
    if(result){
        printf("Tests karime successfull\n");
        return EXIT_SUCCESS;
    }
    else{
        printf("Tests karime failed\n");
        return EXIT_FAILURE;
    }
}
