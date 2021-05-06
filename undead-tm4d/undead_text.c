#include <stdio.h>
#include <stdlib.h>
#include "game.h"

void afficher_grille(game monjeu);
void modifier(game monJeu);
void verifier_contenu(game monJeu,char tab[4][4]);
void viderBuffer();



int main(int argc, char* argv[])
{
 	int x=0,y=0;
	int r=0;
 	unsigned char c=0;
	game monJeu = new_game();
	modifier(monJeu);
	while(!is_game_over(monJeu)){//tanque la solution n'est pas trouvée
		printf("\n\nle format est: <x> <y> <G|V|Z>\n\n");
		afficher_grille(monJeu);//j'affiche la grille
		//verifier_valeurs(monJeu);
		do//tant que le format est invalide
		{//lire un coup sur l'entree standard en exigeant le format: <x> <y> <G|V|Z>
			printf("jouer: \n");
			r = scanf("%d %d %c", &x, &y, &c);
			if(r==EOF)
			{
			  printf("EOF\n");
			  goto end;
			}
			else if(r!=3){
				printf("erreur de format : <x> <y> <G|V|Z>\n");
			}
			viderBuffer();
		}while(r!=3);
			//si le coups n'est pas valide, j'ignore le coup
		if(x<0 || x>3 || y<0 || y>3 || (c!='Z' && c!='G' && c!='V')){
			printf("erreur: x[0,3], y[0,3], Z|G|V\n");
		}
		else//sinon je joue le coup en placant le montre sur la grille
		{
			if(c=='G')
			{
				if(!add_monster(monJeu,GHOST,x,y))
				printf("monstre non ajouté\n");
			}
				else if(c=='V')
			{
				if(!add_monster(monJeu,VAMPIRE,x,y))
				printf("monstre non ajouté\n");
			}
			else
			{
				if(!add_monster(monJeu,ZOMBIE,x,y))
				printf("monstre non ajouté\n");
			}
		}

	}//fin de la boucle du jeu
	end:
	printf("\n\n");
	afficher_grille(monJeu);
	if(is_game_over(monJeu)){
		printf("\n\nBravo vous avez gagné !!\n\n");
	}
	else
	{
		printf("\n\n Desolé, Vous avez perdu la partie...\n\n");
	}
	delete_game(monJeu);
	return EXIT_SUCCESS;
}
void afficher_grille(game monJeu){
	char tab[4][4];
	verifier_contenu(monJeu,tab);
	printf("|   Z:%d V:%d G:%d   |\n", required_nb_monsters(monJeu,ZOMBIE),
									required_nb_monsters(monJeu,VAMPIRE),
									required_nb_monsters(monJeu,GHOST));
	printf("|                 |\n");
	printf("|     %d %d %d %d     |\n", required_nb_seen(monJeu, N, 0),
										required_nb_seen(monJeu, N, 1),
										required_nb_seen(monJeu, N, 2),
										required_nb_seen(monJeu, N, 3));
	printf("|                 |\n");
	printf("|  %d  \\ %c %c /  %d  |\n",required_nb_seen(monJeu,W,3),tab[1][3],tab[2][3],required_nb_seen(monJeu,E,3));
	add_mirror(monJeu,1,0,3);
	add_mirror(monJeu,0,3,3);
	printf("|  %d  \\ %c %c %c  %d  |\n",required_nb_seen(monJeu,W,2),tab[1][2],tab[2][2],tab[3][2],required_nb_seen(monJeu,E,2));
	add_mirror(monJeu,1,0,2);
	printf("|  %d  %c %c \\ /  %d  |\n",required_nb_seen(monJeu,W,1),tab[0][1],tab[1][1],required_nb_seen(monJeu,E,1));
	add_mirror(monJeu,1,2,1);
	add_mirror(monJeu,0,3,1);
	printf("|  %d  \\ %c %c \\  %d  |\n",required_nb_seen(monJeu,W,0),tab[1][0],tab[2][0],required_nb_seen(monJeu,E,0));
	add_mirror(monJeu,1,0,0);
	add_mirror(monJeu,1,3,0);
	printf("|                 |\n");
	printf("|     %d %d %d %d     |\n", required_nb_seen(monJeu, S, 0),
										required_nb_seen(monJeu, S, 1),
										required_nb_seen(monJeu, S, 2),
										required_nb_seen(monJeu, S, 3));

}
void modifier(game monJeu){
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
void verifier_contenu(game monJeu,char tab[4][4]){
	content contenu;
	for(int x=3;x>=0;x--)
	{
		for(int y=0;y<4;y++){
			contenu = get_content(monJeu,y,x);
			if(contenu==EMPTY){
				tab[y][x] = ' ';	
			}
			else if(contenu == MIRROR)
			{
				tab[y][x] = '/';
			}
			else if(contenu == ANTIMIRROR)
			{
				tab[y][x] = '\\';
			}
			else if(contenu == ZOMBIE)
			{
				tab[y][x] = 'Z';
			}
			else if(contenu == VAMPIRE)
			{
				tab[y][x] = 'V';
			}
			else if(contenu == GHOST)
			{
				tab[y][x] = 'G';
			}
		}
		//printf("\n");
	
	}
}
void viderBuffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
    {
        c = getchar();
    }
}  
