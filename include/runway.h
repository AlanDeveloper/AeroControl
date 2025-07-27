#ifndef RUNWAY_H
#define RUNWAY_H

#include <stdbool.h>

typedef struct {
    bool in_use;
    char by_aircraft[10];  // Aircraft ID
} Runway;

#endif
