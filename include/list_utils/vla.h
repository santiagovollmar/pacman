//
// Created by santiago on 10.12.17.
//

#ifndef DBMS_VLA_UTILS_H
#define DBMS_VLA_UTILS_H

typedef struct _vla {
    void* array;
    int size;
    size_t unit;
} Vla;

int vla_create(Vla* vla, size_t unit, int size);
int vla_resize(Vla* vla, int size);
void vla_free(Vla* vla);

#endif //DBMS_VLA_UTILS_H
