#ifndef SIMULATION_H
#define SIMULATION_H

#include <pthread.h>
#include "aircraft.h"
#include "airport.h"

void start(Airport *aircraft, pthread_t *aircraft_threads);

Aircraft* create_aircraft(FlightType type);

void* aircraft_thread_function(void* arg);

#endif
