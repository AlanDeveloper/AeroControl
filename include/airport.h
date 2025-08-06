#ifndef AIRPORT_H
#define AIRPORT_H

#include "runway.h"
#include "gate.h"
#include "aircraft.h"
#include "control_tower.h"

typedef struct {
    Runway *runways;
    int num_runways;

    Gate *gates;
    int num_gates;

    ControlTower *towers;
    int num_towers;

    Aircraft **domestic_aircrafts;
    int num_domestic_aircraft;

    Aircraft **international_aircrafts;
    int num_international_aircraft;

    int operations_by_tower;
} Airport;

#endif
