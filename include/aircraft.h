#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include "flight.h"

typedef struct {
    char id[10];
    FlightType type;
    FlightPhase phase;
} Aircraft;

#endif