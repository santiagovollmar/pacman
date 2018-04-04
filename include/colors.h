#ifndef _COLORS_H_
#define _COLORS_H_

#ifndef __linux__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

/**
 * Function that initialized all the colors
 */
void init_colors(WINDOW *wp);


/*
 * Color pairs
 *  Type:          Pair:       Color-FG:    Color-BG:
 *   - Background -> 1         COLOR_WHITE  COLOR_BLACK
 *   - Wall       -> 10        30           COLOR_BLACK
 *   - Player     -> 2         COLOR_YELLOW COLOR_BLACK
 *   - Ghost 1    -> 5         20           COLOR_BLACK
 *   - Ghost 2    -> 6         21           COLOR_BLACK
 *   - Ghost 3    -> 7         22           COLOR_BLACK
 *   - Ghost 4    -> 8         COLOR_CYAN   COLOR_BLACK
 *   - Ghost vuln -> 9         25           COLOR_BLACK
 */

/*
 * Colors
 *  Type:           RGB:
 *   - COLOR_BLACK  0, 0, 0
 *   - 30           20, 125, 300
 *   - 20           900, 200, 300
 *   - 21           700, 200, 550
 *   - 22           900, 400, 100
 *   - 25           0, 0, 1000
 */

#endif
