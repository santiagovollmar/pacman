#ifndef PACMAN_GUI_
#define PACMAN_GUI_

/**
 * Initialize GUI, has to be called once before updating
 */
void init_gui();

/**
 * The update_gui funtion refreshed the screen
 * @param envChange Bool, if whole environment souhld be reloaded
 */
void update_gui(_Bool envChange);

/**
 * Start the keyboard listener
 */
void start_listener();

#endif
