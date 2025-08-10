#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "aircraft.h"
#include "flight.h"

void init_resources(int runways, int gates, int towers, int max_ops_per_tower);

void destroy_resources(void);

void lock_resources(Aircraft* aircraft);

void unlock_resources(Aircraft* aircraft);

void increment_crashed_planes(void);

void wake_all_threads(void);

#endif
