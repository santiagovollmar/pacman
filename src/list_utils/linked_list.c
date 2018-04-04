#include <malloc.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "../../include/list_utils/linked_list.h"


void* getValue(List* list) {
    void* value = list->currItem->value;
    return value;
}

_Bool hasNext(List* list) {
    _Bool has_next = list->currItem->next != NULL;
    return has_next;
}

_Bool hasPrevious(List* list) {
    _Bool has_previous = list->currItem->previous != NULL;
    return has_previous;
}

//----------modification----------
List* createList() {
    //create new list pointer
    List* list = (List*) malloc(sizeof(List));
    list->index = -1;
    list->size = 0;

    //create first list item
    ListItem* rootItem = (ListItem*) malloc(sizeof(ListItem));
    rootItem->previous = NULL;
    rootItem->next = NULL;
    rootItem->value = NULL;

    //add first item to list
    list->currItem = rootItem;

    return list;
}

void deleteList(List* list) {
    //goto last element
    while (hasNext(list)) {
        gotoNext(list);
    }

    //loop through all elements
    for (int i = list->size; i > -1; --i) {
        //delete value of current item
        free(list->currItem->value);

        //delete current item
        ListItem* itemToBeDeleted = list->currItem;
        list->currItem = itemToBeDeleted->previous;
        free(itemToBeDeleted);
    }

    //delete listpointer
    free(list);
}

void push(List* list, void* valuePtr) {
    //save  current index
    int currIndex = list->index;

    //goto last item
    while (hasNext(list)) {
        gotoNext(list);
    }

    //create new List item
    ListItem* newItem = (ListItem*) malloc(sizeof(ListItem));
    newItem->value = valuePtr;
    newItem->previous = list->currItem;
    newItem->next = NULL;

    //update current item
    list->currItem->next = newItem;

    //update List
    list->currItem = newItem;
    list->size++;
    list->index = list->size - 1;

    //go back to initial index
    //gotoIndex(list, currIndex);
}

void* pop(List* list) {
    if (list->size < 1) {
        return NULL;
    }

    //save  current index
    int currIndex = list->index;

    //goto last item
    while (hasNext(list)) {
        gotoNext(list);
    }

    //remove List Item pointers
    list->currItem->previous->next = NULL;

    //delete List Item
    ListItem* previous = list->currItem->previous;
    void* value = list->currItem->value;
    free(list->currItem);
    list->size--;

    //go back if necessary
    list->currItem = previous;
    list->index--;

    return value;
}

void pushAtIndex(List* list, int index, void* value) {
    if (index >= list->size) {
        push(list, value);
        return;
    }

    //goto requested index
    gotoIndex(list, index);

    //create new List Item
    ListItem* newItem = malloc(sizeof(ListItem));
    newItem->value = value;

    //set pointers of new Item
    newItem->next = list->currItem;
    newItem->previous = list->currItem->previous;

    //set pointers of next
    newItem->next->previous = newItem;

    //set pointers of previous
    newItem->previous->next = newItem;

    //set new Item as current item
    list->currItem = newItem;
    list->size++;
}

void* popAtIndex(List* list, int index) {
    if (index + 1 < list->size) { //item to be removed is not last
        //goto requested index
        gotoIndex(list, index);
        void* value = getValue(list);

        //update previous item
        list->currItem->previous->next = list->currItem->next;

        //update next item
        list->currItem->next->previous = list->currItem->previous;

        //update list
        ListItem* itemToBeRemoved = list->currItem;
        list->currItem = list->currItem->next;
        list->size--;

        //delete item to be deleted
        free(itemToBeRemoved);

        return value;
    } else { //item to be removed is last
        return pop(list);
    }
}
//----------movement----------
void gotoNext(List* list) {
    //check if inside bounds
    if (list->currItem->next == NULL) {
        errno = 1;
        char str[60];
        sprintf(str, "ListOutOfBoundsException in 'gotoNext' --> index = %d", list->index);
        perror(str);
        abort();
    }

    list->currItem = list->currItem->next;
    list->index++;
}

void gotoPrevious(List* list) {
    //check if inside bounds
    if (list->currItem->previous == NULL) {
        errno = 1;
        perror("ListOutOfBoundsException in 'gotoPrevious'");
        abort();
    }

    list->currItem = list->currItem->previous;
    list->index--;
}


void gotoIndex(List* list, int index) {
    //check if inside bounds
    if (list->size <= index || index < -1) {
        errno = 1;
        char error[50] = {0};
        sprintf(error, "ListOutOfBoundsException in 'gotoIndex': %d -> %d", index, list->size);
        perror(error);
        abort();
    }

    //goto correct place
    if (abs(index - list->index) > 0) {
        //get direction
        int direction = index - list->index;

        while (list->index != index) {
            if (direction > 0) {
                gotoNext(list);
            } else if (direction < 0) {
                gotoPrevious(list);
            }
        }
    }
}
