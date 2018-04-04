#ifndef _SKINS_H_
#define _SKINS_H_

#define NUM_OF_SKINS 10

/**
 * Initialize the skins
 */
void init_skins();

struct _skin
{
    int id;

    int bg_color;
    char wall_char;
};
typedef struct _skin Skin;

Skin skins[NUM_OF_SKINS];

#endif
