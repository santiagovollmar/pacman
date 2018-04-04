#ifndef PACMAN_GLOBALS_H
#define PACMAN_GLOBALS_H

#define PACMAN_INIT_SPEED 4
#define GHOST_COOLDOWN 10

#include "pacman_structures.h"

Player player;
Gamestate gamestate;
Ghost ghosts[NUM_OF_GHOSTS];

int points_left;

_Bool inputBlock;
_Bool ghosts_vulnerable;

/*
 *  '=': big point
 *  '-': point
 *  ',': special wall
 *  '0': wall
 *  '.': nothing
 */

char environment[MATRIX_WIDTH][MATRIX_HEIGHT];

/*
 * --> environment[x][y]
 *
 * 4 |
 * 3 |
 * 2 |      q
 * 1 |p
 * 0 |_________________
 *    0 1 2 3 4 5 6 7 8
 *     p = environment[0][1]
 *     q = environment[3][2]
 */

_Bool doExit;
_Bool pause;

#endif
