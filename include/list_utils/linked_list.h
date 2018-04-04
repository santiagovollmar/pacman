//
// Created by Santiago on 14/11/2017.
//

#ifndef FRIDAY_CHALLENGE_LIST_UTILS_H
#define FRIDAY_CHALLENGE_LIST_UTILS_H

struct _listitem {
    void* value;

    struct _listitem* previous;
    struct _listitem* next;
};
typedef struct _listitem ListItem;

struct _listptr {
    int size;
    ListItem* currItem;
    int index;
};
typedef struct _listptr List;

void* getValue(List* list);
_Bool hasNext(List* list);
_Bool hasPrevious(List* list);

void push(List* list, void* valuePtr);
void* pop(List* list);
void pushAtIndex(List* list, int index, void* value);
void* popAtIndex(List* list, int index);

void gotoNext(List* list);
void gotoPrevious(List* list);
void gotoIndex(List* list, int index);

List* createList();
void deleteList(List* list);

#endif //FRIDAY_CHALLENGE_LIST_UTILS_H
