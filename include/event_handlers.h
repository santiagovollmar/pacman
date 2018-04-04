#ifndef PACMAN_EVENT_HANDLERS_H
#define PACMAN_EVENT_HANDLERS_H

#include "pacman_structures.h"
#include "globals.h"

/**
 * Player Model collision reaction
 * @param t     Type of collider
 * @param other Type of other collider
 */
void onPlayerCollision(Type t, void* other);

/**
 * Ghost Model collision reaction
 * @param ghost ghost
 * @param t     type of ghost
 * @param other other collider
 */
void onGhostCollision(Ghost* ghost);

/**
 * Called by the keylistener on key down
 * @param  keycode Code of the key
 */
void *onKeyDown(int keycode);

#endif
