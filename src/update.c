#include <math.h>
#include <time.h>
#ifndef __linux__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif
#include <pthread.h>
#include "../include/sleep.h"
#include "../include/update.h"
#include "../include/pacman_structures.h"
#include "../include/event_handlers.h"
#include "../include/globals.h"
#include "../include/pacman_gui.h"
#include "../include/path_finder.h"

#define SPEED_RATE ((1000.0/FRAMES_PER_SECOND) * 0.001)

struct _position {
    double x;
    double y;
};
typedef  struct _position Position;

Position getPositionChange(Type t, void* obj);
void getNewPosition(int* arr, Position* posChange, _Bool reset);

int getHspeed(Type t, void* instance);
int getVspeed(Type t, void* instance);

Position playerPosChange = {0};
Position ghostsPosChange[NUM_OF_GHOSTS] = {{0}};

pthread_t event;

/**
 * Main function that manages the game
 */
void runUpdater() {
    int sleepTime = floor(1000/FRAMES_PER_SECOND);

    while(!doExit) {
        if(!pause)
        {
            clock_t begin = clock();

            //attroff(COLOR_PAIR(200));
            inputBlock = 1;

            update();
            update_gui(1);

            inputBlock = 0;

            clock_t end = clock();
            sleep_ms(sleepTime - ((end - begin)*1000 / CLOCKS_PER_SEC));
            // TODO: Handle negative value exception
        }
    }
}

void input_listener()
{
    while(!doExit)
    {
        flushinp();
        int key_raw = getch();

        if(key_raw == '\033')
        {
            // Is arrow
            getch();
            int key_special = getch();
            #ifdef __DEBUG__
            mvprintw(0, 0, "%d", key_special);
            #endif
            pthread_t keyDown;
            pthread_create(&keyDown, NULL, onKeyDown, NULL);
            onKeyDown(key_special);
        }
        else
        {
            // Is letter or digit
            #ifdef __DEBUG__
            mvprintw(0, 0, "%d", key_raw);
            #endif
            onKeyDown(key_raw);
        }
    }
}

/**
 * Updates positions and detects collsions
 * Called by the runUpdate function
 */
void update() {
    /**
     * set change of positions
     */
    {   //player
        Position p = getPositionChange(PLAYER, NULL);
        playerPosChange.x += p.x;
        playerPosChange.y += p.y;
    }

    /*
     * calculate new positions
     */
    static int move_ghosts = 0;
    if(move_ghosts > 1)
    {
        moveGhosts();
        move_ghosts = 0;
    }
    move_ghosts++;

    Position oldPosChangePlayer = {0};
    oldPosChangePlayer.x = playerPosChange.x;
    oldPosChangePlayer.y = playerPosChange.y;

    int newPlayerPos[2] = {player.x_pos, player.y_pos};
#ifdef __DEBUG__
    mvprintw(1, 0, "Player now X: %d", newPlayerPos[0]);
    mvprintw(2, 0, "Player now Y: %d", newPlayerPos[1]);
#endif
    {   //player
        getNewPosition(newPlayerPos, &playerPosChange, 0);
    }
#ifdef __DEBUG__
    mvprintw(3, 0, "Player new X: %d\n", newPlayerPos[0]);
    mvprintw(4, 0, "Player new Y: %d\n", newPlayerPos[1]);
#endif

    /*
     * check for colliding objects
     */
     _Bool doAnimation = 1;
    {   //environment
        char envCode = environment[newPlayerPos[0]][newPlayerPos[1]];
#ifdef __DEBUG__
        mvprintw(14, 0, "Collision check X: %d", newPlayerPos[0]);
        mvprintw(15, 0, "Collision check Y: %d", newPlayerPos[1]);
#endif
        Type t = NOTHING;


        switch(envCode) {
            case ENV_WALL:
            case ENV_SPECIAL_WALL:
                // Collision with wall
                t = WALL;
                doAnimation = 0;
                break;

            case ENV_SIDE_WALL:
                // Collision with teleporter
                t = TELE;
                break;

            case ENV_POINT:
                // Collision with small food
                t = SMALLPOINT;
                break;

            case ENV_BIG_POINT:
                // Collision with big food
                t = BIGPOINT;
                break;
        }

        if (t != NOTHING) {
            onPlayerCollision(t, &environment[newPlayerPos[0]] [newPlayerPos[1]]);
        }
    }

    /*
     * pacman and ghost animations
     */
    {   //player
        if(doAnimation)
        {
            player.state = player.state ? 0 : 1;
        }
    }
    {   //ghosts
        for(int i = 0; i < NUM_OF_GHOSTS; i++)
        {
            ghosts[i].state = ghosts[i].state ? 0 : 1;
        }
    }

    /*
     * recalculate positions
     */
    playerPosChange.x = oldPosChangePlayer.x;
    playerPosChange.y = oldPosChangePlayer.y;

    {   //player
        newPlayerPos[0] = player.x_pos;
        newPlayerPos[1] = player.y_pos;
        getNewPosition(newPlayerPos, &playerPosChange, 1);
    }
#ifdef __DEBUG__
    mvprintw(12, 0, "Before rec. playerX: %d\n", newPlayerPos[0]);
    mvprintw(13, 0, "Before rec. playerY %d\n", newPlayerPos[1]);
#endif

    /*
     * update positions
     */
    {   //player
        player.x_pos = newPlayerPos[0];
        player.y_pos = newPlayerPos[1];
    }


    {   //ghosts
        for (int i = 0; i < NUM_OF_GHOSTS; i++) {
            _Bool x_match = player.x_pos == ghosts[i].x_pos;
            _Bool y_match = player.y_pos == ghosts[i].y_pos;

            if (x_match && y_match) {
                onGhostCollision(&ghosts[i]);
            }
        }
    }
}

/**
 * Adds position change to arr
 * @param arr       [description]
 * @param posChange Buffer for position
 * @param reset     Reset position
 */
void getNewPosition(int* arr, Position* posChange, _Bool reset) {
    _Bool resetChange[2] = {0, 0};

    if (posChange->x >= 0.5) {
        arr[0]++;
        resetChange[0] = 1;
    } else if (posChange->x <= -0.5) {
        arr[0]--;
        resetChange[0] = 1;
    }

    if (posChange->y >= 0.5) {
        arr[1]++;
        resetChange[1] = 1;
    } else if (posChange->y <= -0.5) {
        arr[1]--;
        resetChange[1] = 1;
    }
    posChange->x = (resetChange[0] && reset) ? 0 : posChange->x;
    posChange->y = (resetChange[1] && reset) ? 0 : posChange->x;
}

/**
 * Returns the change in position of a specific object
 * @param  t   Type of object whoms position changed
 * @param  obj Object
 * @return     New position
 */
Position getPositionChange(Type t, void* obj) {
    Position p = {0};
    switch (t) {
        case PLAYER:
            p.x = ((double) getHspeed(PLAYER, NULL))*SPEED_RATE;
            p.y = ((double) getVspeed(PLAYER, NULL))*SPEED_RATE;
            break;

        case GHOST:
            p.x = ((double) getHspeed(GHOST, obj))*SPEED_RATE;
            p.y = ((double) getVspeed(GHOST, obj))*SPEED_RATE;

        default:
            return p;
    }

    return p;
}

/**
 * Returns vertical speed of a given instance (or the player)
 * @param  t        Type of the collider
 * @param  instance Instance of the collider
 * @return          Returns speed
 */
int getVspeed(Type t, void* instance) {
    switch (t) {
        case PLAYER:
            switch(player.direction) {
                case UP:
                    return -player.speed;

                case DOWN:
                    return player.speed;

                default:
                    return 0;
            }

        case GHOST:
            switch(((Ghost*) instance)->direction) {
                case UP:
                    return -((Ghost*) instance)->speed;

                case DOWN:
                    return ((Ghost*) instance)->speed;

                default:
                    return 0;
            }

        default:
            return 0;
    }
}

/**
 * Returns horizontal speed of a given instance (or the player)
 * @param  t        Type of the collider
 * @param  instance Instance of the collider
 * @return          Returns speed
 */
int getHspeed(Type t, void* instance) {
    switch (t) {
        case PLAYER:
            switch(player.direction) {
                case RIGHT:
                    return player.speed;

                case LEFT:
                    return -player.speed;

                default:
                    return 0;
            }

        case GHOST:
            switch(((Ghost*) instance)->direction) {
                case RIGHT:
                    return ((Ghost*) instance)->speed;

                case LEFT:
                    return -((Ghost*) instance)->speed;

                default:
                    return 0;
            }

        default:
            return 0;
    }
}
