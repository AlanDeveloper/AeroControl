#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <pthread.h>
#include "aircraft.h"

void resource_manager_init(int num_runways, int num_towers, int num_gates);
void resource_manager_destroy(void);
void lock_resources(Aircraft* aircraft);
void unlock_resources(Aircraft* aircraft);

#endif
