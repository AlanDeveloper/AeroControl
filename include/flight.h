#ifndef FLIGHT_H
#define FLIGHT_H

typedef enum {
    DOMESTIC,
    INTERNATIONAL
} FlightType;

typedef enum {
    PHASE_NONE,
    BOARDING,
    TAKEOFF,
    LANDING,
    DEBOARDING
} FlightPhase;

#endif
