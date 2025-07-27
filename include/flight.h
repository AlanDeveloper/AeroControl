#ifndef FLIGHT_H
#define FLIGHT_H

typedef enum {
    DOMESTIC,
    INTERNATIONAL
} FlightType;

typedef enum {
    PHASE_NONE,
    LANDING,
    DEBOARDING,
    TAKEOFF
} FlightPhase;

#endif
