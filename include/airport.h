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

    Aircraft *aircrafts;
    int num_aircrafts;
} Airport;

#endif
