#ifndef PACMAN_GUI_INTERNAL_H_
#define PACMAN_GUI_INTERNAL_H_

/* Default Sprites */
#define GHOST_SPRITE    " ### # # ############ # #"

#define PACMAN_SPRITE_S1_R   " /##\\|# #/|##| |###\\ \\##/ "
#define PACMAN_SPRITE_S1_L   "/##\\ \\# #| |##|/###|\\##/ "
#define PACMAN_SPRITE_S1_U   "/\\_/\\###### ########\\###/ "
#define PACMAN_SPRITE_S1_D   " ___ /###\\### ###_##\\/ \\/"

#define PACMAN_SPRITE_S2_R   " /#\\ |# #\\|##|||###/ \\#/  "
#define PACMAN_SPRITE_S2_L   " /#\\ /# #||###|\\###| \\#/  "
#define PACMAN_SPRITE_S2_U   " /|\\ /###\\# ########\\###/ "
#define PACMAN_SPRITE_S2_D   " ___ /###\\### #\\#|#/ \\|/  "

#define POINT_DEFAULT_POINT         '$'
#define POINT_BIG_DEFAULT_SPRITE    "      /#\\  ###  \\#/      "
#define THIN_WALL_DEFAULT_SPRITE    " ===  ===  ===  ===  === "

#define SCALE 5

/* Global sprites */
char chr_wall;
char sprt_big_point[27];
char sprt_ghost[27];
char sprt_thin_wall[27];

/* General functions */
void key_listener();
void init_map();
void get_values();
void init_sprites();

/**
 * Update the local environment
 */
void getMap();
/**
 * Update the global environment
 */
void setMap();

/**
 * Draw a singe point
 * @param origX Left top X origin
 * @param origY Left top Y origin
 * @param chr   Char for point
 * @param color Color of point
 */
void drawPoint(int *origX, int *origY, char chr, int color);

/**
 * Draw a wall
 * @param currX Left top X origin
 * @param currY Left top Y origin
 * @param chr   Char for wall
 * @param color Color for wall
 */
void drawWall(int *currX, int *currY, char chr, int color);

/**
 * Draw a block (pacman, ghost, etc)
 * @param origX Left top X origin
 * @param origY Left top Y origin
 * @param array Array that defines block
 * @param color Color for block
 */
void drawBlock(int *origX, int *origY, char* array, int color);

/* Global map variables */
char map[29][29];
int rows;
int cols;

#endif
