//
// Created by santiago on 10.12.17.
//

#include <glob.h>
#include <malloc.h>
#include "vla.h"

int vla_create(Vla* vla, size_t unit, int size) {
    if (size < 0) {
        return -1;  //report illegal value
    }

    vla->size = size;
    vla->unit = unit;
    if (size) {
        vla->array = (void*)malloc(unit * size);
    } else {
        vla->array = NULL;
    }
}

int vla_resize(Vla* vla, int size) {
    if (size < 1) {
        return -1;  //report illegal value
    }

    if (vla->size) {
        //just found out that this could have been done with realloc :(
        //copy contents of vla into local array
        size_t vla_bytes = vla->size * vla->unit;
        char loc_array[vla_bytes];
        for (int i = 0; i < vla_bytes; i++) {
            loc_array[i] = ((char *) vla->array)[i];
        }

        //reallocate vla array
        size_t nvla_bytes = vla->unit * size;
        free(vla->array);
        vla->array = malloc(nvla_bytes);

        //copy contents into newly allocated memory
        for (int i = 0; i < nvla_bytes; ++i) {
            if (i < vla_bytes) {
                ((char *) vla->array)[i] = loc_array[i];
            } else {
                ((char *) vla->array)[i] = 0;
            }
        }
    } else {
        vla_create(vla, vla->unit, size);
    }

    return 0;
}

void vla_free(Vla* vla) {
    //update vla
    vla->size = 0;

    //free memory
    if (vla->size) {
        free(vla->array);
    }
    vla->array = NULL;
}

