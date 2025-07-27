#ifndef GATE_H
#define GATE_H

#include <stdbool.h>

typedef struct {
    bool in_use;
    char by_aircraft[10];  // Aircraft ID
} Gate;

#endif
