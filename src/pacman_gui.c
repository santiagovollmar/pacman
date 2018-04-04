#include <stdlib.h>
#include <stdio.h>
#ifndef __linux__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif
#include <locale.h>
#include <wchar.h>
#include <math.h>
#include <string.h>

/* Own dependencies */
#include "../include/sleep.h"
#include "../include/pacman_structures.h"
#include "../include/globals.h"
#include "../include/event_handlers.h"

/* Implementations */
#include "../include/pacman_gui.h"
#include "../include/pacman_gui_internal.h"
#include "../include/colors.h"
#include "../include/skins.h"

void init_gui()
{
    // Initialize locale; not needed anymore
    setlocale(LC_ALL, "en_US.UTF-8");

    // Init the NCurses window
    WINDOW *wp = initscr();
    noecho();
    getmaxyx(stdscr, rows, cols);		/* get the number of rows and colsumns */

    // Init colors and map
    init_sprites();
    init_colors(wp);
    init_skins();
    init_map();


    // Deactivate the cursor
    curs_set(0);

    // Load local map into global map
    setMap();

    return;
}
void start_listener()
{
    // Wait for keyboard input
    while(!doExit)
    {
        flushinp();
        int key = getch();

        onKeyDown(key);

        sleep_ms(1000);
    }
}

/* Load file contents into map */
void init_map()
{
    FILE *fp = fopen("maze.txt", "r");

    int indexX = 0;
    int indexY = 0;

    /* Read file and load into local map */
    char c = 0;
    while((c = fgetc(fp)) != EOF)
    {
        if(c == '\n')
        {
            // New Y coordinate
            indexY++;
            indexX = 0;
        }
        else
        {
            // New entry
            map[indexY][indexX] = c;
            // New X coordinate
            indexX++;
        }
    }
    fclose(fp);
}

/**
 * The update_gui funtion refreshed the screen
 * @param envChange Bool, if whole environment souhld be reloaded
 */
void update_gui(_Bool envChange)
{
    if(envChange)
    {
        // Load global map into local map
        getMap();
    }

    int padding_wid = (cols/2) - (29*SCALE/2);

    int currX = padding_wid;
    int currY = 1;

    int isCharacter = 0;

    points_left = 0;

    int playerX = player.x_pos;
    int playerY = player.y_pos;

    for(int y = 0; y < 29; y++)
    {
        for(int x = 0; x < 29; x++)
        {
            /* Check player or ghost */
            if(playerX == x && playerY == y)
            {
                // Set player
                char sprite[30] = {0};
                // Define state
                switch(player.direction)
                {
                    case UP:
                        strcpy(sprite, (player.state ? PACMAN_SPRITE_S1_U : PACMAN_SPRITE_S2_U));
                        break;
                    case DOWN:
                        strcpy(sprite, (player.state ? PACMAN_SPRITE_S1_D : PACMAN_SPRITE_S2_D));
                        break;
                    case RIGHT:
                        strcpy(sprite, (player.state ? PACMAN_SPRITE_S1_R : PACMAN_SPRITE_S2_R));
                        break;
                    case LEFT:
                        strcpy(sprite, (player.state ? PACMAN_SPRITE_S1_L : PACMAN_SPRITE_S2_L));
                        break;
                }

                drawBlock(&currX, &currY, sprite, 2);
                isCharacter = 1;
            }
            else
            {
                for(int i = 0; i < NUM_OF_GHOSTS; i++)
                {
                    if(ghosts[i].x_pos == x && ghosts[i].y_pos == y)
                    {
                        // Set ghost
                        int color = ghosts[i].color;
                        if(ghosts_vulnerable)
                        {
                            // Ghosts are vulnerable
                            color = ghosts[i].state ? 9 : 1;
                        }
                        drawBlock(&currX, &currY, sprt_ghost, color);
                        isCharacter = 1;
                        break;
                    }
                }
            }

            // Is no player or ghost; draw environment
            if(!isCharacter)
            {
                char c = map[y][x];
                switch(c)
                {
                    case '0':
                        drawWall(&currX, &currY, chr_wall, 10);
                        break;
                    case '-':
                        drawPoint(&currX, &currY, POINT_DEFAULT_POINT, 1);
                        points_left++;
                        break;
                    case '=':
                        drawBlock(&currX, &currY, sprt_big_point, 1);
                        break;
                    case '.':
                        drawPoint(&currX, &currY, ' ', 2);
                        break;
                    case ',':
                        drawBlock(&currX, &currY, sprt_thin_wall, 2);
                        break;
                    case '|':
                        drawWall(&currX, &currY, ';', 3);
                        break;
                }
            }
            isCharacter = 0;
        }
        currY += SCALE;
        currX = padding_wid;
    }
    #ifdef __DEBUG__
    mvprintw(7, 0, "Points left: %d", points_left);
    #endif
    refresh();
}

/* Initialize default sprites */
void init_sprites()
{
    chr_wall = POINT_DEFAULT_POINT;
    strcpy(sprt_big_point, POINT_BIG_DEFAULT_SPRITE);
    strcpy(sprt_ghost, GHOST_SPRITE);
    strcpy(sprt_thin_wall, THIN_WALL_DEFAULT_SPRITE);
}

/* Map management */
void setMap()
{
    for(int x = 0; x < 29; x++)
    {
        for(int y = 0; y < 29; y++)
        {
            environment[x][y] = map[y][x];
        }
    }
}
void getMap()
{
    for(int x = 0; x < 29; x++)
    {
        for(int y = 0; y < 29; y++)
        {
            map[x][y] = environment[y][x];
        }
    }
}

/* Drawing functions */
void drawWall(int *origX, int *origY, char chr, int color)
{
    char array[1] = {chr};
    int currX = *origX;
    int currY = *origY;

    attron(COLOR_PAIR(color));
    for(int i = 0; i < SCALE; i++)
    {
        for(int ii = 0; ii < SCALE; ii++)
        {
            mvprintw(currY+i, currX+ii, array);
        }
    }
    *origX += SCALE;

    attroff(COLOR_PAIR(color));
}
void drawPoint(int *origX, int *origY, char chr, int color)
{
    char* padding = " ";
    char array[1] = {chr};
    int currX = *origX;
    int currY = *origY;

    int counter = 0;
    int center = (SCALE*SCALE)/2 + ((SCALE*SCALE) - 2*((SCALE*SCALE)/2));
    //clear();
    //printw("Center: %d\nHalf: %d\nHalf Real: %d\nDiff: %d\n", center, (SCALE*SCALE)/2, SCALE,
    //        ((SCALE*SCALE) - 2*((SCALE*SCALE)/2)));

    attron(COLOR_PAIR(color));

    for(int i = 0; i <= SCALE; i++)
    {
        for(int ii = 0; ii <= SCALE; ii++)
        {
            counter++;
            if(counter-i == center)
            {
                mvprintw(currY + i, currX + ii, array);
            }
            else
            {
                mvprintw(currY + i, currX + ii, padding);
            }
        }
    }
    attroff(COLOR_PAIR(color));

    *origX += SCALE;
}
void drawBlock(int *origX, int *origY, char* array, int color)
{
    int currX = *origX;
    int currY = *origY;

    int counter = 0;

    attron(COLOR_PAIR(color));
    for(int i = 0; i < SCALE; i++)
    {
        for(int ii = 0; ii < SCALE; ii++)
        {
            char arr[1] = {array[counter]};
            mvprintw(currY+i, currX+ii, arr);
            counter++;
        }
    }
    *origX += SCALE;

    attroff(COLOR_PAIR(color));
}
