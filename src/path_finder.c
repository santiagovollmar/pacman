#include <limits.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "../include/path_finder.h"
#include "../include/list_utils/linked_list.h"
#include "../include/pacman_structures.h"
#include "../include/globals.h"
#include "../include/sleep.h"

struct _position {
    int x;
    int y;
};
typedef struct _position Position;

struct _node {
    Position position;

    int g_cost; //distance start <-> node
    int h_cost; //distance end <-> node (without obstacles)
    int f_cost; //g + h

    struct _node *parent;
};
typedef struct _node Node;

struct _cd_args {
    int ms;
    Ghost* ghost;
};

Node* explore(int ghost, Position destination);
void getNeighbours(Position* arr, Position center);
Position getNextMove(Node *endNode, int layer);
void resetPathFinder();
int listContainsAt(List* list, Position p);
_Bool isWall(Position p, int index);
int taxicabDistance(Position start, Position end);
void *waitCooldown(struct _cd_args *args);

void initPathFinder();

List *open[NUM_OF_GHOSTS];
List *closed[NUM_OF_GHOSTS];

Node *endNodes[NUM_OF_GHOSTS];

_Bool locked_move = 0;
_Bool locked_update = 0;

int layer = 1;
int ghostDifficulty = 5;

void setGhostMovableCooldown(int ms, Ghost *ghost) {
    struct _cd_args *args_p = (struct _cd_args*)malloc(sizeof(struct _cd_args));
    args_p->ms = ms;
    args_p->ghost = ghost;

    pthread_t waitCooldown_t;
    pthread_create(&waitCooldown_t, NULL, (void*)waitCooldown, args_p);
}

void *waitCooldown(struct _cd_args *args) {
    sleep_ms(args->ms);
    args->ghost->movable = 1;
    return NULL;
}

void setGhostDifficulty(int difficulty) {
	if (difficulty <= 10 && difficulty >= 1) {
		ghostDifficulty = difficulty;
	} else if (difficulty > 10) {
		ghostDifficulty =  10;
	} else if (difficulty < 1) {
		ghostDifficulty = 1;
	}
}

void runExplorer() {
    initPathFinder();

    for (int i = 0; i < NUM_OF_GHOSTS; i++) {
        ghosts[i].movable = 0;
    }

	while (!doExit && !locked_update) {
		locked_move = 1;	//lock usage of open and closed
        resetPathFinder();
		Position playerPosition = {player.x_pos, player.y_pos};

		for (int i = 0; i < NUM_OF_GHOSTS; i++) {
            if (ghosts_vulnerable) {

                /*do {
                    playerPosition.x = rand() % (MATRIX_WIDTH - 1);
                    playerPosition.y = rand() % (MATRIX_HEIGHT - 1);
                } while (!isWall(playerPosition) && ghosts_vulnerable);*/
                // TODO: Add ghosts vulnerability case
            }
			endNodes[i] = explore(i, playerPosition);
		}
		locked_move = 0;	//unlock usage of open and  closed
		layer = 0;
		sleep_ms(100);
	}
}


void moveGhosts() {
	for (int i = 0; i < NUM_OF_GHOSTS; i++) {
        if (!ghosts[i].movable) {
            continue;
        }
		if (!locked_move) {
            locked_update = 1;
            if(endNodes[i] != NULL)
            {
    			Position p = getNextMove(endNodes[i], layer);

                if (ghosts_vulnerable) {
                    int diffX = ghosts[i].x_pos - p.x;
                    int diffY = ghosts[i].y_pos - p.y;

                    ghosts[i].x_pos -= diffX;
                    ghosts[i].y_pos -= diffY;
                }
                else
                {
                    ghosts[i].x_pos = p.x;
                    ghosts[i].y_pos = p.y;
                }

                /*for(int j = 0; j < NUM_OF_GHOSTS; j++)
                {
                    if(i != j)
                    {
                        if(ghosts[j].x_pos == p.x && ghosts[j].y_pos == p.y)
                        {

                        }
                    }
                }*/
            }
            locked_update = 0;
		}
	}

	layer++;
}

/**
 * explores possible paths with the A* algorythm and returns the end node
 * @param ghost
 * @param destination
 * @return
 */
Node* explore(int ghost, Position destination) {
    while (1) {
        //get Node with lowest f_cost
        int index = 0;
        int smallest = INT_MAX;
        Node *current = NULL;

        gotoIndex(open[ghost], -1);
        while (hasNext(open[ghost])) {
            gotoNext(open[ghost]);
            Node *n = (Node *) getValue(open[ghost]);
            if (n->f_cost < smallest) {
                index = open[ghost]->index;
                current = n;
                smallest = n->f_cost;
            }
        }


        if(current == NULL)
        {
            //TODO fix this
            return NULL;
        }

        //remove current from open and add to closed
        popAtIndex(open[ghost], index);
        push(closed[ghost], current);

        //check if destination has been reached
        if (current->position.x == destination.x && current->position.y == destination.y) {
            return current;
        }

        //get neighbours
        Position neighbourPositions[4] = {0};
        getNeighbours(neighbourPositions, current->position);
        Node* neighbours[4] = {0};
        for (int i = 0; i < 4; i++) {
            //allocate memory
            neighbours[i] = (Node*) malloc(sizeof(Node));
            memset(neighbours[i], 0, sizeof(Node));

            //set costs
            neighbours[i]->position = neighbourPositions[i];
            neighbours[i]->h_cost = taxicabDistance(neighbourPositions[i], destination);
            neighbours[i]->g_cost = current->g_cost + 1;
            neighbours[i]->f_cost = neighbours[i]->h_cost + neighbours[i]->g_cost;

            //set parent
            neighbours[i]->parent = current;
        }

        //check neighbors
        for (int i = 0; i < 4; i++) {
            //check if neighbor is traversable and not in closed
            if (isWall(neighbourPositions[i], ghost)) {
				free(neighbours[i]);
                continue;
            } else if (listContainsAt(closed[ghost], neighbourPositions[i]) > -1) {
                continue;
			}

            //check if neighbor is in open
            int indexInOpen = listContainsAt(open[ghost], neighbourPositions[i]);
            if (indexInOpen > -1) {
                //check if new neighbour is closer to ghost than current one
                Node *currOpenNode = (Node*) getValue(open[ghost]);
                if (neighbours[i]->g_cost < currOpenNode->g_cost) { //improvement
                    //swap Nodes
                    free(popAtIndex(open[ghost], indexInOpen)); //old node is no longer needed
                    pushAtIndex(open[ghost], indexInOpen, neighbours[i]);
                } else {    //no improvement
                    //neighbour is no longer needed
                    free(neighbours[i]);
                }
            } else {
                pushAtIndex(open[ghost], 0, neighbours[i]);
            }
        }
    }
}

/**
 * writes coordinates of the direct neighbours of center into arr in the following order:
 *  - top
 *  - bottom
 *  - left
 *  - right
 * @param arr
 * @param center
 */
void getNeighbours(Position* arr, Position center) {
    {   //top
        Position p = {0};
        p.x = center.x;
        p.y = center.y - 1;
        arr[0] = p;
    }

    {   //bottom
        Position p = {0};
        p.x = center.x;
        p.y = center.y + 1;
        arr[1] = p;
    }

    {   //left
        Position p = {0};
        p.x = center.x - 1;
        p.y = center.y;
        arr[2] = p;
    }

    {   //right
        Position p = {0};
        p.x = center.x + 1;
        p.y = center.y;
        arr[3] = p;
    }
}

/**
 * returns the best next move for a ghost
 * @return
 */
Position getNextMove(Node *endNode, int layer) {
	Node *currNode = endNode;

	if (currNode->g_cost <= layer) {
		return endNode->position;
	}

	while (currNode->parent->g_cost != layer) {
		currNode = currNode->parent;
	}

	return currNode->position;
}

/**
 * initializes all required values for the path finding algorythm
 */
void initPathFinder() {
    for (int i = 0; i < NUM_OF_GHOSTS; i++) {
        //create open and closed lists
        open[i] = createList();
        closed[i] = createList();

        // wait time between 100ms and 2100ms
        int wait_time = 100 + 500*i;
        setGhostMovableCooldown(wait_time, &ghosts[i]);

        //create starting node for each ghost
        Node *start = (Node*) malloc(sizeof(Node));
		Position startPosition = {ghosts[i].x_pos, ghosts[i].y_pos};
		start->position = startPosition;

		Position playerPosition = {player.x_pos, player.y_pos};
		start->h_cost = taxicabDistance(start->position, playerPosition);
		start->g_cost = 0;
		start->f_cost = start->h_cost;

		start->parent = NULL;

        //push starting node to open list
        push(open[i], start);
    }
}

/**
 * resets all required values for the path finding algorythm to their standard values
 */
void resetPathFinder() {
    //delete each list
    for (int i = 0; i < NUM_OF_GHOSTS; i++) {
        deleteList(open[i]);
        deleteList(closed[i]);

        open[i] = NULL;
        closed[i] = NULL;
    }

    //create lists again
    initPathFinder();
}

/**
 * returns the index at which a given Position is contained within a linked list.
 * if the Value is not contained within it, -1 is returned
 * @param list
 * @param p
 * @return
 */
int listContainsAt(List* list, Position p) {
    gotoIndex(list, -1);
    while (hasNext(list)) {
        gotoNext(list);
        Node *curr = (Node*) getValue(list);

        if (curr->position.x == p.x && curr->position.y == p.y) {
            return list->index;
        }
    }

    return -1;
}

/**
 * returns a boolean stating wether or not, there is a wall at the given position
 * @param p
 * @return
 */
_Bool isWall(Position p, int index) {
    // Check if is wall
    char envCode = environment[p.x][p.y];
    switch (envCode) {
         case ENV_WALL:
         case ENV_SIDE_WALL:
             return 1;
    }

    // Check if is ghost
    for (int i = 0; i < NUM_OF_GHOSTS; ++i) {
        if(i == index)
        {
            // Own ghost
            continue;
        }

        // Get neighbours of current ghost position
        Position ghostNeighbours[4] = {0};
        Position ghostPos = { ghosts[i].x_pos, ghosts[i].y_pos };
        getNeighbours(ghostNeighbours, ghostPos);

        // Get neighbours of next ghost position
        ghostPos = ghostNeighbours[ghosts[i].direction];
        getNeighbours(ghostNeighbours, ghostPos);

        // Go through all next ghost position neighbours
        for(int ii = 0; ii < 4; ii++)
        {
            if (ghostNeighbours[ii].x == p.x && ghostNeighbours[ii].y == p.y) {
#ifdef __DEBUG__
                mvprintw(20+1, 0, "Ghost collision: %d with %d at %d", index, i, ii);
#endif
                return 1;
            }
        }

    }
    return 0;
}

/**
 * returns the taxicab-Distance (Manhattan-Distance) between two points
 * @param start
 * @param end
 * @return
 */
int taxicabDistance(Position start, Position end) {
    return abs(start.x - end.x) + abs(start.y - end.y);
}

//depreceated --> no diagonal movement
int euklideanDistance(Position start, Position end) {
    return sqrtl(powl((start.x - end.x), 2) + powl((start.y - end.y), 2));
}
