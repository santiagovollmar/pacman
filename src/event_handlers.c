#include "../include/event_handlers.h"
#ifndef __linux__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#include "../include/sleep.h"
#include "../include/path_finder.h"

void easteregg();
void *ghost_cooldown();
void game_over();
void game_won();
void start_cooldown();

/**
 * Player Model collision reaction
 * @param t     Type of collider
 * @param other Type of other collider
 */
void onPlayerCollision(Type t, void* other) {
    switch (t) {
        case SMALLPOINT:
            gamestate.points += 10;
            *((char*) other) = ENV_NOTHING;
            // Check if there are no points left
            if(points_left <= 1)
            {
                // Game won
                gamestate.won = 1;
                game_won();
            }
            break;

        case BIGPOINT:
            // Start cooldown
            start_cooldown();
            gamestate.points += 50;
            *((char*) other) = ENV_NOTHING;
            break;

        case WALL:
            switch(player.direction)
            {
                case UP:
                    player.y_pos++;
                    break;
                case DOWN:
                    player.y_pos--;
                    break;
                case LEFT:
                    player.x_pos++;
                    break;
                case RIGHT:
                    player.x_pos--;
                    break;
            }
            break;
        case TELE:
            // Next field is teleporter
#ifdef __DEBUG__
            mvprintw(20, 0, "Pos: %d", player.x_pos);
#endif
            if(player.x_pos >= 26)
            {
                player.x_pos = 0;
                player.direction = RIGHT;
            }
            else if(player.x_pos <= 1)
            {
                player.x_pos = 27;
                player.direction = LEFT;
            }
            player.y_pos = 13;
            break;

        default:
            break;
    }
    return;
}

/**
 * Game over ascii art
 */
void game_over()
{
    //return;
    endwin();
    doExit = 1;
    printf("  /$$$$$$                                                                                 \n");
    printf(" /$$__  $$                                                                                \n");
    printf("| $$  \\__/  /$$$$$$  /$$$$$$/$$$$   /$$$$$$         /$$$$$$  /$$    /$$ /$$$$$$   /$$$$$$ \n");
    printf("| $$ /$$$$ |____  $$| $$_  $$_  $$ /$$__  $$       /$$__  $$|  $$  /$$//$$__  $$ /$$__  $$\n");
    printf("| $$|_  $$  /$$$$$$$| $$ \\ $$ \\ $$| $$$$$$$$      | $$  \\ $$ \\  $$/$$/| $$$$$$$$| $$  \\__/\n");
    printf("| $$  \\ $$ /$$__  $$| $$ | $$ | $$| $$_____/      | $$  | $$  \\  $$$/ | $$_____/| $$      \n");
    printf("|  $$$$$$/|  $$$$$$$| $$ | $$ | $$|  $$$$$$$      |  $$$$$$/   \\  $/  |  $$$$$$$| $$      \n");
    printf(" \\______/  \\_______/|__/ |__/ |__/ \\_______/       \\______/     \\_/    \\_______/|__/      \n");
    printf("                                                                                          \n");
    printf("                                                                                          \n");
    printf("                                                                                          \n");
    exit(0);
}
/**
 * Game won ASCII art
 */
void game_won()
{
    endwin();
    doExit = 1;
    printf(" /$$     /$$                                      /$$          \n");
    printf("|  $$   /$$/                                     |__/          \n");
    printf(" \\  $$ /$$//$$$$$$  /$$   /$$       /$$  /$$  /$$ /$$ /$$$$$$$ \n");
    printf("  \\  $$$$//$$__  $$| $$  | $$      | $$ | $$ | $$| $$| $$__  $$\n");
    printf("   \\  $$/| $$  \\ $$| $$  | $$      | $$ | $$ | $$| $$| $$  \\ $$\n");
    printf("    | $$ | $$  | $$| $$  | $$      | $$ | $$ | $$| $$| $$  | $$\n");
    printf("    | $$ |  $$$$$$/|  $$$$$$/      |  $$$$$/$$$$/| $$| $$  | $$\n");
    printf("    |__/  \\______/  \\______/        \\_____/\\___/ |__/|__/  |__/\n");
    printf("                                                               \n");
    printf("                                                               \n");
    printf("                                                               \n");
    exit(0);
}

/**
 * Ghost Model collision reaction
 * @param ghost ghost
 * @param t     type of ghost
 * @param other other collider
 */
void onGhostCollision(Ghost* ghost) {
    if (ghosts_vulnerable) {
        ghost->movable = 0;
        ghost->x_pos = ghost->spawn_x;
        ghost->y_pos = ghost->spawn_y;

        setGhostMovableCooldown(1000, ghost);
    } else {
        gamestate.gameover = 1;
        game_over();
    }
}

/**
 * This function is called, if a keydown was detected by the listener
 * @param  keycode Code of key
 */
void *onKeyDown(int keycode)
{
#ifdef __DEBUG__
    mvprintw(10, 0, "Keypress: %d", keycode);
#endif
    // End pause pon every key press
    pause = 0;
    if(inputBlock)
    {
        return;
    }
    switch(keycode)
    {
        case 72:
        case 65:
        case 119:
            player.speed = PACMAN_INIT_SPEED;
            if(environment[player.x_pos][player.y_pos-1] != ENV_WALL)
            {
                player.direction = UP;
            }
            break;
        case 66:
        case 115:
            player.speed = PACMAN_INIT_SPEED;
            if(environment[player.x_pos][player.y_pos+1] != ENV_WALL)
            {
                player.direction = DOWN;
            }
            break;
        case 75:
        case 68:
        case 97:
            player.speed = PACMAN_INIT_SPEED;
            if(environment[player.x_pos-1][player.y_pos] != ENV_WALL)
            {
                player.direction = LEFT;
            }
            break;
        case 77:
        case 67:
        case 100:
            player.speed = PACMAN_INIT_SPEED;
            if(environment[player.x_pos+1][player.y_pos] != ENV_WALL)
            {
                player.direction = RIGHT;
            }
            break;
        case 80:
        case 112:
            pause = 1;
            break;
        case 114:
            clear();
            break;
        case 99:
            endwin();
            //! If this macro is defined, there will be no eastereggs
#ifndef __FALL__
            easteregg();
#endif
            break;
        case 113:
            endwin();
            doExit = 1;
            break;
#ifdef __DEBUG__
        case 32:
            //player.speed = player.speed ? 0 : PACMAN_INIT_SPEED;
            pause = pause ? 0 : 1;
            break;
        case 121:
            endwin();
            doExit = 1;
            game_over();
            break;
        case 120:
            endwin();
            doExit = 1;
            game_won();
            break;
#endif
    }
}

/**
 * Easteregg, just for fun if key "c" was pressed
 */
void easteregg()
{
    int color = 1;

    for(int i = 0; i < 300; i++)
    {
        if (color)
        {
            #ifdef __linux
            system("echo -n \"\033[30;49m\"");
            #else
            system("color 04");
            #endif
            color--;
        }
        else
        {
            #ifdef __linux
            system("echo -n \"\033[39;40m\"");
            #else
            system("color 0F");
            #endif
            color++;
        }
        printf("|| [Abandon ship!] || %*s \n", i, " ");
        sleep_ms(10);
    }
    doExit = 1;
    sleep_ms(100);
    errno = 1;
    printf("\n");
    perror("Abandon ship!!\n");
    abort();
}

/**
 * Ghost vulnerability phase countdown
 */
void *ghost_cooldown()
{
    ghosts_vulnerable = 1;
    sleep_ms(9000);
    ghosts_vulnerable = 0;
    return NULL;
}

/**
 * Ghost vulnerability countdown starter
 */
void start_cooldown()
{
    pthread_t cooldown;
    pthread_create(&cooldown, NULL, ghost_cooldown, NULL);
}
