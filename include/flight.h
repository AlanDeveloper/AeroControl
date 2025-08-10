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
    FLY,
    LANDING,
    DEBOARDING
} FlightPhase;

const char* get_phase_label_pt(FlightPhase phase);

#endif
