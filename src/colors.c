#include "../include/colors.h"

void init_colors(WINDOW *wp)
{
    /* Start color defining */
        start_color();
    /* Redefine black */
        init_color(COLOR_BLACK, 0, 0, 0);

    /* Background */
        init_pair(1, COLOR_WHITE, COLOR_BLACK);

    /* Player */
        init_pair(2, COLOR_YELLOW, COLOR_BLACK);

    /* Ghosts */
        // Magenta
        init_color(20, 900, 200, 300);
        init_pair(5, 20, COLOR_BLACK);
        // Pink
        init_color(21, 700, 200, 550);
        init_pair(6, 21, COLOR_BLACK);
        // Orange
        init_color(22, 900, 400, 100);
        init_pair(7, 22, COLOR_BLACK);
        // Cyan
        init_pair(8, COLOR_CYAN, COLOR_BLACK);

    /* Ghost vulnerable */
        init_color(25, 0, 0, 1000);
        init_pair(9, 25, COLOR_BLACK);

    /* Walls */
        init_color(30, 20, 125, 300);
        //init_color(30, 700, 50, 300);
        init_pair(10, 30, COLOR_BLACK);

    /* Optional skin colors */
        init_color(50, 30, 30, 30);
        init_color(51, 40, 40, 40);
        init_color(52, 40, 40, 40);
        init_color(53, 50, 50, 50);
        init_color(54, 60, 60, 60);
        init_color(55, 70, 70, 70);
        init_color(56, 80, 80, 80);
        init_color(57, 90, 90, 90);
        init_color(58, 100, 100, 100);
        init_color(59, 110, 110, 110);

    wbkgd(wp, COLOR_PAIR(1));
}
