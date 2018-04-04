#ifndef PACMAN_PATH_FINDER_H
#define PACMAN_PATH_FINDER_H

#include "pacman_structures.h"

/**
 * Set difficulty
 * @param difficulty Difficulty
 */
void setGhostDifficulty(int difficulty);

/**
 * Start path finding
 */
void runExplorer();

/**
 * Update the ghosts' positions
 */
void moveGhosts();

/**
 * Hold the ghosts in place for a certain time
 * @param ms    Time to hold
 * @param ghost Ghost to hold
 */
void setGhostMovableCooldown(int ms, Ghost *ghost);

#endif
