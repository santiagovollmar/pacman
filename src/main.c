#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#ifndef __linux__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif
#include "../include/sleep.h"
#include "../include/pacman_structures.h"
#include "../include/update.h"
#include "../include/pacman_gui.h"
#include "../include/globals.h"
#include "../include/path_finder.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *keyboard_listen();
void *start_path_finding();
void init_game();
void init_player();
void init_ghosts();

int main() {
    init_gui();
    init_game();
    init_player();
    init_ghosts();

    /* Start Keyboard listener */
    pthread_t keyboard_listener;
    pthread_mutex_lock(&mutex);
    pthread_create(&keyboard_listener, NULL, keyboard_listen, NULL);
    pthread_mutex_unlock(&mutex);


    pthread_t pathFinder;
    pthread_mutex_lock(&mutex);
    pthread_create(&pathFinder, NULL, start_path_finding, NULL);
    pthread_mutex_unlock(&mutex);

    update_gui(1);
    runUpdater();

    endwin();
    return 0;
}
void init_game()
{
    ghosts_vulnerable = 0;
    doExit = 0;
    pause = 1;

    gamestate.gameover = 0;
    gamestate.won = 0;
    gamestate.level = 0;
    gamestate.points = 0;
}
void init_player()
{
    player.x_pos = 9;
    player.y_pos = 16;
    player.direction = RIGHT;
    player.speed = 0;
    player.state = 0;
}
void init_ghosts()
{
    for (int i = 0; i < NUM_OF_GHOSTS; i++) {
        ghosts[i].x_pos = 12 + i;
        ghosts[i].y_pos = 13 + 2*((i % 2) - 1) + 1;
        ghosts[i].spawn_x = ghosts[i].x_pos;
        ghosts[i].spawn_y = ghosts[i].y_pos;
        ghosts[i].direction = DOWN;
        ghosts[i].speed = 2;
        ghosts[i].color = i+5;
    }
}
void *keyboard_listen()
{
    input_listener();
    return NULL;
}
void *start_path_finding()
{
    runExplorer();
    return NULL;
}
