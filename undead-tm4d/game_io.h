#ifndef GAME_IO_H
#define GAME_IO_H
#include "game.h"

/**
 * @file game_io.h
 *
 * @brief This file provides functions to load or save a game.
 *
 * 
 * @detail The file format used to save game instance is a simple text
 * file format, which stores integer values (separated by a single
 * space) or ASCII char for the board content {'.','/', '\', '|', '-',
 * 'V', 'G', 'Z', 'S'}. Note that the character '.' represents an
 * EMPTY content.
 *
 * File format description:
 * 
 @verbatim
 <width> <height>
 <required_nb_vampires> <required_nb_ghosts> <required_nb_zombies> <required_nb_spirits>
 <labels[N][0]> <labels[N][1]> ... <labels[N][width-1]>
 <labels[S][0]> <labels[S][1]> ... <labels[S][width-1]>
 <labels[E][0]> <labels[E][1]> ... <labels[E][height-1]>
 <labels[W][0]> <labels[W][1]> ... <labels[W][height-1]>
 <board[0][height-1]> <board[1][height-1]> ... <board[width-1][height-1]>
 ...
 <board[0][1]> <board[1][1]> ... <board[width-1][1]>
 <board[0][0]> <board[1][0]> ... <board[width-1][0]>
 @endverbatim
 * 
 * Here is an example of the initial state of a simple 4x4 game:
 @verbatim
 4 4
 2 2 5 0
 0 3 3 0
 0 3 2 3
 0 0 3 2
 0 2 3 3
 \ . . /
 \ . . .
 . . \ /
 \ . . \
 @endverbatim

*/

///@{

/**
 * @brief Creates a game by loading its description in a file
 *
 * @param filename
 * @return the loaded game
 **/
game load_game(char* filename);

/**
 * @brief Save a game in a file
 *
 * @param g game to save
 * @param filename
 **/
void save_game(cgame g, char* filename);

///@}

#endif // GAME_IO_H
