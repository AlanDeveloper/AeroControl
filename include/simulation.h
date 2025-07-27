#ifndef SIMULATION_H
#define SIMULATION_H

#include "aircraft.h"

void start(int *num_runways, int *num_gates, int *num_towers, int *operations_by_tower, int *num_domestic_aircraft, int *num_international_aircraft, pthread_t *aircraft_threads);

Aircraft* create_aircraft(FlightType type);

void* aircraft_thread_function(void* arg);

#endif
