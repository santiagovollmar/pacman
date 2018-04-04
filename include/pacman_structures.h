#ifndef PACMAN_PACMAN_STRUCTURES_H
#define PACMAN_PACMAN_STRUCTURES_H

#define NUM_OF_GHOSTS 4
#define MATRIX_WIDTH 29
#define MATRIX_HEIGHT 29

enum _envcodes {
    ENV_BIG_POINT = '=',
    ENV_POINT = '-',
    ENV_SPECIAL_WALL = ',',
    ENV_SIDE_WALL = '|',
    ENV_WALL = '0',
    ENV_NOTHING = '.'
};

enum _type {
    PLAYER,
    GHOST,
    SMALLPOINT,
    BIGPOINT,
    WALL,
    TELE,
    NOTHING
};

enum _direction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};

enum _color {
    magenta = 5,    //! 20
    pink = 6,       //! 21
    orange = 7,     //! 22
    cyan = 8        //! COLOR_CYAN
};

typedef enum _envcodes EnvironmentCodes;
typedef enum _type Type;
typedef enum _direction Direction;
typedef enum _color Color;

struct _gamestate {
    char nickname[16];
    _Bool gameover;
    _Bool won;
    int level;
    int points;

    int vulnerabilityPhase;
    int ghostsThisVP;
};

struct _playerscore {
    char nickname[16];
    char date[20];
    int score;
    int pacBucks;
    int unlockedSkins[10];

    struct _playerscore *next;
};

struct _player {
    int x_pos;
    int y_pos;

    Direction direction;
    int speed;

    _Bool state;
};

struct _ghost {
    int x_pos;
    int y_pos;

    int spawn_x;
    int spawn_y;

    Direction direction;
    int speed;
    Color color;
    char weak;

    _Bool movable;
    _Bool state;
};

typedef struct _gamestate Gamestate;
typedef struct _playerscore Playerscore;
typedef struct _player Player;
typedef struct _ghost Ghost;

#endif
