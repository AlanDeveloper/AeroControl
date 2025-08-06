#ifndef SIMULATION_H
#define SIMULATION_H

#include <pthread.h>
#include "aircraft.h"
#include "airport.h"

void start(Airport *airport, pthread_t *aircraft_threads);

void allocate_resources(Airport *airport);
void initialize_resources(Airport *airport);
void create_and_start_aircraft_threads(Airport *airport, pthread_t *aircraft_threads);
void print_airport_summary(Airport *airport);

Aircraft* create_aircraft(FlightType type);

void* aircraft_thread_function(void* arg);

#endif
