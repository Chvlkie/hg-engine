#ifndef MAP_MATRIX_H
#define MAP_MATRIX_H

#include "types.h"

#define MAP_MATRIX_MAX_SIZE        799
#define MAP_MATRIX_MAX_NAME_LENGTH 16

typedef struct MAPDATA {
    u16 models[MAP_MATRIX_MAX_SIZE];
} MAPDATA;

typedef struct MAPMATRIXDATA {
    u8 height;
    u8 width;
    u16 headers[MAP_MATRIX_MAX_SIZE];
    u8 altitudes[MAP_MATRIX_MAX_SIZE];
    MAPDATA maps;
    u8 name[MAP_MATRIX_MAX_NAME_LENGTH];
} MAPMATRIXDATA;

typedef struct MAPMATRIX {
    u8 width;
    u8 height;
    u8 matrix_id;
    MAPMATRIXDATA data;
} MAPMATRIX;
#endif