#include <stdio.h>
#include <stdlib.h>
#include "game.h"  //fichier h contenant les fonctions a implementer

#define N 4


/*
int main(int argc,char *argv[])
{
	printf("elhadjium007\n");
	return EXIT_SUCCESS;
}
//*/
typedef struct nb_requis{
  int ghost;
  int vampire;
  int zombie;
} s_nb_requis;
struct game_s{
  content squares[N][N];  //tableau de content
  s_nb_requis nb_requis;  //nombre de monstre requis
  s_nb_requis nb_current_monster; // nombre de monstres courant
  int labels[N][N];
};

void mettreAJourMonstre(content contenu,bool *mirror,bool *antimirror,int* compteur);
int sud_current_nb_seen(int pos,cgame game);
//################
game new_game()
{
    game monJeu =(game)malloc(sizeof(struct game_s));
    if(!monJeu)
    {
      fprintf(stderr,"L'allocation pour le jeu à echoué\n");
      exit(EXIT_FAILURE);
    }
    //Mise de tous les contents à empty
   for(int i=0;i<N;i++)
   {
     for(int j=0;j<N;j++)
     {
       monJeu->squares[i][j] = EMPTY; //je met toutes les cases à 0
     }
   }
   //mettre les required_nb_seen à 0
   monJeu->nb_requis.ghost = 0;
   monJeu->nb_requis.vampire = 0;
   monJeu->nb_requis.zombie = 0;
   //mise de tous les labels à 0

   for(int i=0;i<N;i++)
   {
     for(int j=0;j<N;j++)
     {
       monJeu->labels[i][j]=0;
     }
   }
   //initialisation du monstres courants à 0
   monJeu->nb_current_monster.ghost = 0;
   monJeu->nb_current_monster.vampire = 0;
   monJeu->nb_current_monster.zombie = 0;

    return monJeu;
}
//################
void add_mirror(game game, int dir, int col, int line)
{
  if(!game)
  {
    fprintf(stderr,"pointeur null dans  add_mirror\n");
    exit(EXIT_FAILURE);
  }
  if(col<0 || col>3 || line<0 || line>3 || (dir!=0 && dir!=1))
  {
    fprintf(stderr,"col et line prennent des valeurs comprises entre 0 et 3\nL'argument dir prend les valeurs 0 ou 1");
    exit(EXIT_FAILURE);
  }
  if(dir==0)
    game->squares[line][col] = MIRROR;
  else
    game->squares[line][col] = ANTIMIRROR;
}
//##############
void set_required_nb_seen(game game, direction side, int pos, int value)
{
  if(!game)
  {
    fprintf(stderr,"pointeur null dans set_required_nb_seen\n");
    exit(EXIT_FAILURE);
  }
  /*
  if(side!=N && side!=S && side!=E && side!=W)
  {
    fprintf(stderr,"Dans set_required_nb_seen, Le param side doit être N|S|E|W\n");
    exit(EXIT_FAILURE);
  }
  */
  if(pos<0 || pos>3)
  {
    fprintf(stderr, "Le param pos est compris entre 0 et 3\n");
    exit(EXIT_FAILURE);
  }
  if(value<0)
  {
    fprintf(stderr,"Le param ne peut pas être negatif\n");
    exit(EXIT_FAILURE);
  }
  if(side==N)
    game->labels[0][pos]=value;
  else if(side==E)
    game->labels[1][pos]=value;
  else if(side==W)
    game->labels[2][pos]=value;
  else
    game->labels[3][pos]=value;
}
//##############
void set_required_nb_monsters(game game, content monster, int value)
{
  if(!game)
  {
    fprintf(stderr,"le parametre game de set_riquired_nb_monstre ne doit pas être null\n");
    exit(EXIT_FAILURE);
  }
  if(monster!=GHOST && monster!=VAMPIRE && monster!=ZOMBIE)
  {
    fprintf(stderr, "Dans set_required_nb_monsters, l'argument monstre doit être un ZOMBIE|GHOST|VAMPIRE\n");
    exit(EXIT_FAILURE);
  }
  if(value<0)
  {
    fprintf(stderr,"Dans set_required_nb_monsters, l'argument value ne peut pas être négatif\n");
    exit(EXIT_FAILURE);
  }
  if(monster==ZOMBIE)
      game->nb_requis.zombie=value;
  else if(monster==VAMPIRE)
      game->nb_requis.vampire=value;
  else
      game->nb_requis.ghost=value;
}
//############
game copy_game(cgame g_src)
{

}
//###############
void delete_game(game g)
{
  if(g!=NULL)
  {
    free(g);
    g=NULL;
  }
}
//###############
int required_nb_seen(cgame game, direction side, int pos)
{
  if(!game)
  {
    fprintf(stderr,"Dans required_nb_seen, le parametre game ne doit pas être null\n");
    exit(EXIT_FAILURE);
  }
  /*
  if(side!=N && side!=S && side!=E && side!=W)
  {
    fprintf(stderr,"Dans required_nb_seen, Le param side doit être N|S|E|W\n");
    exit(EXIT_FAILURE);
  }
  */
  if(pos<0 || pos>3)
  {
    fprintf(stderr, "Le param pos de required_nb_seen est compris entre 0 et 3\n");
    exit(EXIT_FAILURE);
  }
  int nombre=0;
  if(side==N)
    nombre = game->labels[0][pos];
  else if(side==E)
    nombre = game->labels[1][pos];
  else if(side==W)
    nombre = game->labels[2][pos];
  else
    nombre = game->labels[3][pos];
  return nombre;
}
//###############
content get_content(cgame game, int col, int line)
{
  if(!game)
  {
    fprintf(stderr,"pointeur null dans get_content\n");
    exit(EXIT_FAILURE);
  }
  if(col<0 || col>3 || line<0 || line>3)
  {
    fprintf(stderr,"col et line prennent des valeurs comprises entre 0 et 3\n");
    exit(EXIT_FAILURE);
  }
  return game->squares[line][col];
}
//##############
int required_nb_monsters(cgame game, content monster)
{
  if(!game)
  {
    fprintf(stderr,"le parametre game de riquired_nb_monstre ne doit pas être null\n");
    exit(EXIT_FAILURE);
  }
  if(monster!=GHOST && monster!=VAMPIRE && monster!=ZOMBIE)
  {
    fprintf(stderr, "Dans required_nb_monsters, l'argument monstre doit être un ZOMBIE|GHOST|VAMPIRE\n");
    exit(EXIT_FAILURE);
  }
  int nombre=0;
  if(monster==ZOMBIE)
      nombre = game->nb_requis.zombie;
  else if(monster==VAMPIRE)
      nombre = game->nb_requis.vampire;
  else
      nombre = game->nb_requis.ghost;
  return nombre;
}
//##############
bool is_game_over(cgame g)
{

}
//##############
void restart_game(game g)
{

}
//################
bool add_monster(game game, content monster, int col, int line)
{
  if(!game)
  {
    fprintf(stderr, "Le pointeur passé a add_monster est null\n");
    exit(EXIT_FAILURE);
  }
  if(monster!=GHOST && monster!=VAMPIRE && monster!=ZOMBIE)
  {
    fprintf(stderr, "Dans add_monster, l'argument monstre doit être un ZOMBIE|GHOST|VAMPIRE\n");
    exit(EXIT_FAILURE);
  }
  if(col<0 || col>3 || line<0 || line>3)
  {
    fprintf(stderr,"Dans add_monster, col et line prennent des valeurs comprises entre 0 et 3\n");
    exit(EXIT_FAILURE);
  }
  if(game->squares[line][col]==MIRROR || game->squares[line][col]==ANTIMIRROR)
  {
    return false;
  }
  game->squares[line][col] = monster;
  //mise à jour du nombre de monstres courant
  if(monster==ZOMBIE)
    game->nb_current_monster.zombie+=1;
  else if(monster==VAMPIRE)
    game->nb_current_monster.vampire+=1;
  else
    game->nb_current_monster.ghost+=1;
  return true;
}
//#############
int current_nb_seen(cgame game, direction side, int pos)
{
    if(!game)
    {
      fprintf(stderr, "pointeur null\n");
      exit(EXIT_FAILURE);
    }
    if(pos<0 || pos>3)
    {
      fprintf(stderr, "param pos\n");
      exit(EXIT_FAILURE);
    }
    int i=pos;
    int j=3;
    bool mirror=false, antimirror=false;
    int compteur=0;
    content contenu;
    bool continuer = true;
    bool debut = true;
    /*
    switch (side) {
      case N:

        break;
      case S:

        break;
      case E:

        break;
      case W:

        break;
    }
    */
    if(side==N)   // ici ça me donne false meme si je passe N en param
    {
      do
      {
        if(!debut){ // je ne teste pas si suis a pos
          if((i==3 && j==3) || (i==1 && j==-1) || (i==0 && j==3)){
            continuer = false;
            break;
          }
        }
        debut = false;
        contenu = get_content(game,i,j); //je recupere le contenu
        if(contenu == MIRROR)
          mirror = true;
        if(contenu == ANTIMIRROR)
          antimirror = true;
        //je determine la prochaine case;
          if(mirror && !antimirror)
            i--;
          else if(!mirror && antimirror)
            i++;
          else if(!mirror && !antimirror)
            j--;
          else if(mirror && antimirror)
            j++;
        //je met à jour la compteur
        if(contenu==ZOMBIE)
          compteur++;
        else if(contenu==VAMPIRE)
        {
          if(!mirror && !antimirror)
          {
            compteur++;
          }
        }
        else if(contenu==GHOST)
        {
          if(mirror || antimirror)
          {
            compteur++;
          }
        }
      }while(continuer);
    }
    else if(1==1)
    {
      int res=-1;
      res = sud_current_nb_seen(pos,game);
      return res;
    }
    else if(side==E)
    {
      printf("coté est\n");
    }
    else if(side==W)
    {
      printf("coté ouest\n");
    }
    return compteur;
}
//############
int current_nb_monsters(cgame game, content monster)
{
  if(!game)
  {
    fprintf(stderr,"pointeur null\n");
    exit(EXIT_FAILURE);
  }
  switch (monster) {
    case VAMPIRE:
                    return game->nb_current_monster.vampire;
    case ZOMBIE:
                    return game->nb_current_monster.zombie;
    case GHOST:
                    return game->nb_current_monster.ghost;
    default:
                    fprintf(stderr, "le parma monster doit être ZOMBIE|GHOST|VAMPIRE\n");
                    exit(EXIT_FAILURE);
  }
}
//###########
game setup_new_game(int *labels[NB_DIR],content *board, int required_nb_ghosts, int required_nb_vampires, int required_nb_zombies)
{

}
//---------------------
int sud_current_nb_seen(int pos,cgame g)
{
  int i=pos, j=0, compteur=0, nombre_anti_mirror=0;
  bool mirror=false, antimirror=false;
  content contenu;
  bool continuer = true, debut=true;
  do{
    if(!debut)
    {
      if((i==-1 && j==0) || (i==1 && j==4) || (i==-1 && j==1) || (i==-1 && j==2)){
        continuer = false;
        break;
      }
    }
    debut = false;
    contenu = get_content(g,i,j); //je recupere le contenu
    if(contenu == ANTIMIRROR)
      nombre_anti_mirror++;
    //je met à jour le nombre de monstres courant
    mettreAJourMonstre(contenu,&mirror,&antimirror,&compteur);
    //je determine la prochaine case
    if(mirror && !antimirror)
      i++;
    else if(!mirror && antimirror){
      if(nombre_anti_mirror%2==0)
        j++;
      else
        i--;
    }
    else if(!mirror && !antimirror)
      j++;
  }while(continuer);
  return compteur;
}
void mettreAJourMonstre(content contenu,bool *mirror,bool *antimirror,int* compteur)
{
  if(contenu == MIRROR)
    *mirror = true;
  if(contenu == ANTIMIRROR)
    *antimirror = true;
  if(contenu==ZOMBIE)
    (*compteur)++;
  else if(contenu==VAMPIRE)
  {
    if(!mirror && !antimirror)
    {
      (*compteur)++;
    }
  }
  else if(contenu==GHOST)
  {
    if(mirror || antimirror)
    {
      (*compteur)++;
    }
  }
}
