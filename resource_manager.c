#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#include "include/resource_manager.h"

static int runways_total, towers_total, gates_total;

static pthread_mutex_t *runways_mutex;
static pthread_mutex_t *towers_mutex; 
static pthread_mutex_t *gates_mutex;

static sem_t runways_sem;
static sem_t towers_sem;
static sem_t gates_sem;

static int lock_resource(pthread_mutex_t *mutexes, sem_t *sem, int total, const char *name) {
    sem_wait(sem);

    for (int i = 0; i < total; i++) {
        if (pthread_mutex_trylock(&mutexes[i]) == 0) {
            return i;
        }
    }
    return -1;
}

static void unlock_resource(pthread_mutex_t *mutexes, sem_t *sem, int index) {
    pthread_mutex_unlock(&mutexes[index]);
    sem_post(sem);
}

typedef struct {
    int runway_idx;
    int tower_idx;
    int gate_idx;
} ResourceAllocation;

static __thread ResourceAllocation current_allocation = { -1, -1, -1 };
static __thread char current_aircraft_id[10] = {0};

void resource_manager_init(int num_runways, int num_towers, int num_gates) {
    runways_total = num_runways;
    towers_total = num_towers;
    gates_total = num_gates;

    runways_mutex = malloc(runways_total * sizeof(pthread_mutex_t));
    towers_mutex = malloc(towers_total * sizeof(pthread_mutex_t));
    gates_mutex = malloc(gates_total * sizeof(pthread_mutex_t));

    sem_init(&runways_sem, 0, runways_total);
    sem_init(&towers_sem, 0, towers_total);
    sem_init(&gates_sem, 0, gates_total);

    for (int i = 0; i < runways_total; i++)
        pthread_mutex_init(&runways_mutex[i], NULL);
    for (int i = 0; i < towers_total; i++)
        pthread_mutex_init(&towers_mutex[i], NULL);
    for (int i = 0; i < gates_total; i++)
        pthread_mutex_init(&gates_mutex[i], NULL);
}

void resource_manager_destroy() {
    for (int i = 0; i < runways_total; i++)
        pthread_mutex_destroy(&runways_mutex[i]);
    for (int i = 0; i < towers_total; i++)
        pthread_mutex_destroy(&towers_mutex[i]);
    for (int i = 0; i < gates_total; i++)
        pthread_mutex_destroy(&gates_mutex[i]);

    free(runways_mutex);
    free(towers_mutex);
    free(gates_mutex);

    sem_destroy(&runways_sem);
    sem_destroy(&towers_sem);
    sem_destroy(&gates_sem);
}

static void lock_boarding_domestic() {
    current_allocation.gate_idx = lock_resource(gates_mutex, &gates_sem, gates_total, "Gate");
    current_allocation.tower_idx = lock_resource(towers_mutex, &towers_sem, towers_total, "Tower");

    printf("[RECURSO] Avião %-5s ► Alocou portão %d, torre %d para embarque DOMÉSTICO\n",
           current_aircraft_id, current_allocation.gate_idx, current_allocation.tower_idx);
}

static void unlock_boarding_domestic() {
    printf("[RECURSO] Avião %-5s ◄ Liberou torre %d, portão %d do embarque DOMÉSTICO\n",
           current_aircraft_id, current_allocation.tower_idx, current_allocation.gate_idx);

    unlock_resource(towers_mutex, &towers_sem, current_allocation.tower_idx);
    unlock_resource(gates_mutex, &gates_sem, current_allocation.gate_idx);
    current_allocation.gate_idx = current_allocation.tower_idx = -1;
}

static void lock_boarding_international() {
    current_allocation.tower_idx = lock_resource(towers_mutex, &towers_sem, towers_total, "Tower");
    current_allocation.gate_idx = lock_resource(gates_mutex, &gates_sem, gates_total, "Gate");

    printf("[RECURSO] Avião %-5s ► Alocou torre %d, portão %d para embarque INTERNACIONAL\n",
           current_aircraft_id, current_allocation.tower_idx, current_allocation.gate_idx);
}

static void unlock_boarding_international() {
    printf("[RECURSO] Avião %-5s ◄ Liberou portão %d, torre %d do embarque INTERNACIONAL\n",
           current_aircraft_id, current_allocation.gate_idx, current_allocation.tower_idx);

    unlock_resource(gates_mutex, &gates_sem, current_allocation.gate_idx);
    unlock_resource(towers_mutex, &towers_sem, current_allocation.tower_idx);
    current_allocation.gate_idx = current_allocation.tower_idx = -1;
}

static void lock_landing_domestic() {
    current_allocation.tower_idx = lock_resource(towers_mutex, &towers_sem, towers_total, "Tower");
    current_allocation.runway_idx = lock_resource(runways_mutex, &runways_sem, runways_total, "Runway");

    printf("[RECURSO] Avião %-5s ► Alocou torre %d, pista %d para pouso DOMÉSTICO\n",
           current_aircraft_id, current_allocation.tower_idx, current_allocation.runway_idx);
}

static void unlock_landing_domestic() {
    printf("[RECURSO] Avião %-5s ◄ Liberou torre %d, pista %d do pouso DOMÉSTICO\n",
           current_aircraft_id, current_allocation.tower_idx, current_allocation.runway_idx);

    unlock_resource(runways_mutex, &runways_sem, current_allocation.runway_idx);
    unlock_resource(towers_mutex, &towers_sem, current_allocation.tower_idx);
    current_allocation.runway_idx = current_allocation.tower_idx = -1;
}

static void lock_landing_international() {
    current_allocation.runway_idx = lock_resource(runways_mutex, &runways_sem, runways_total, "Runway");
    current_allocation.tower_idx = lock_resource(towers_mutex, &towers_sem, towers_total, "Tower");

    printf("[RECURSO] Avião %-5s ► Alocou pista %d, torre %d para pouso INTERNACIONAL\n",
           current_aircraft_id, current_allocation.runway_idx, current_allocation.tower_idx);
}

static void unlock_landing_international() {
    printf("[RECURSO] Avião %-5s ◄ Liberou torre %d, pista %d do pouso INTERNACIONAL\n",
           current_aircraft_id, current_allocation.tower_idx, current_allocation.runway_idx);

    unlock_resource(towers_mutex, &towers_sem, current_allocation.tower_idx);
    unlock_resource(runways_mutex, &runways_sem, current_allocation.runway_idx);
    current_allocation.runway_idx = current_allocation.tower_idx = -1;
}

static void lock_deboarding_domestic() {
    current_allocation.gate_idx = lock_resource(gates_mutex, &gates_sem, gates_total, "Gate");
    current_allocation.tower_idx = lock_resource(towers_mutex, &towers_sem, towers_total, "Tower");

    printf("[RECURSO] Avião %-5s ► Alocou portão %d, torre %d para desembarque DOMÉSTICO\n",
           current_aircraft_id, current_allocation.gate_idx, current_allocation.tower_idx);
}

static void unlock_deboarding_domestic() {
    printf("[RECURSO] Avião %-5s ◄ Liberou torre %d, portão %d do desembarque DOMÉSTICO\n",
           current_aircraft_id, current_allocation.tower_idx, current_allocation.gate_idx);

    unlock_resource(towers_mutex, &towers_sem, current_allocation.tower_idx);
    unlock_resource(gates_mutex, &gates_sem, current_allocation.gate_idx);
    current_allocation.gate_idx = current_allocation.tower_idx = -1;
}

static void lock_deboarding_international() {
    current_allocation.tower_idx = lock_resource(towers_mutex, &towers_sem, towers_total, "Tower");
    current_allocation.gate_idx = lock_resource(gates_mutex, &gates_sem, gates_total, "Gate");

    printf("[RECURSO] Avião %-5s ► Alocou torre %d, portão %d para desembarque INTERNACIONAL\n",
           current_aircraft_id, current_allocation.tower_idx, current_allocation.gate_idx);
}

static void unlock_deboarding_international() {
    printf("[RECURSO] Avião %-5s ◄ Liberou portão %d, torre %d do desembarque INTERNACIONAL\n",
           current_aircraft_id, current_allocation.gate_idx, current_allocation.tower_idx);

    unlock_resource(gates_mutex, &gates_sem, current_allocation.gate_idx);
    unlock_resource(towers_mutex, &towers_sem, current_allocation.tower_idx);
    current_allocation.gate_idx = current_allocation.tower_idx = -1;
}

static void lock_takeoff_domestic() {
    current_allocation.tower_idx = lock_resource(towers_mutex, &towers_sem, towers_total, "Tower");
    current_allocation.gate_idx = lock_resource(gates_mutex, &gates_sem, gates_total, "Gate");
    current_allocation.runway_idx = lock_resource(runways_mutex, &runways_sem, runways_total, "Runway");

    printf("[RECURSO] Avião %-5s ► Alocou torre %d, portão %d, pista %d para decolagem DOMÉSTICA\n",
           current_aircraft_id, current_allocation.tower_idx, current_allocation.gate_idx, current_allocation.runway_idx);
}

static void unlock_takeoff_domestic() {
    printf("[RECURSO] Avião %-5s ◄ Liberou pista %d, portão %d, torre %d da decolagem DOMÉSTICA\n",
           current_aircraft_id, current_allocation.runway_idx, current_allocation.gate_idx, current_allocation.tower_idx);

    unlock_resource(runways_mutex, &runways_sem, current_allocation.runway_idx);
    unlock_resource(gates_mutex, &gates_sem, current_allocation.gate_idx);
    unlock_resource(towers_mutex, &towers_sem, current_allocation.tower_idx);
    current_allocation.runway_idx = current_allocation.gate_idx = current_allocation.tower_idx = -1;
}

static void lock_takeoff_international() {
    current_allocation.gate_idx = lock_resource(gates_mutex, &gates_sem, gates_total, "Gate");
    current_allocation.runway_idx = lock_resource(runways_mutex, &runways_sem, runways_total, "Runway");
    current_allocation.tower_idx = lock_resource(towers_mutex, &towers_sem, towers_total, "Tower");

    printf("[RECURSO] Avião %-5s ► Alocou portão %d, pista %d, torre %d para decolagem INTERNACIONAL\n",
           current_aircraft_id, current_allocation.gate_idx, current_allocation.runway_idx, current_allocation.tower_idx);
}

static void unlock_takeoff_international() {
    printf("[RECURSO] Avião %-5s ◄ Liberou torre %d, pista %d, portão %d da decolagem INTERNACIONAL\n",
           current_aircraft_id, current_allocation.tower_idx, current_allocation.runway_idx, current_allocation.gate_idx);

    unlock_resource(towers_mutex, &towers_sem, current_allocation.tower_idx);
    unlock_resource(runways_mutex, &runways_sem, current_allocation.runway_idx);
    unlock_resource(gates_mutex, &gates_sem, current_allocation.gate_idx);
    current_allocation.runway_idx = current_allocation.gate_idx = current_allocation.tower_idx = -1;
}

void lock_resources(Aircraft* aircraft) {
    strncpy(current_aircraft_id, aircraft->id, sizeof(current_aircraft_id) - 1);
    current_aircraft_id[sizeof(current_aircraft_id) - 1] = '\0';
    
    switch (aircraft->phase) {
        case BOARDING:
            if (aircraft->type == INTERNATIONAL)
                lock_boarding_international();
            else
                lock_boarding_domestic();
            break;
        case LANDING:
            if (aircraft->type == INTERNATIONAL)
                lock_landing_international();
            else
                lock_landing_domestic();
            break;
        case DEBOARDING:
            if (aircraft->type == INTERNATIONAL)
                lock_deboarding_international();
            else
                lock_deboarding_domestic();
            break;
        case TAKEOFF:
            if (aircraft->type == INTERNATIONAL)
                lock_takeoff_international();
            else
                lock_takeoff_domestic();
            break;
        default:
            break;
    }
}

void unlock_resources(Aircraft* aircraft) {
    switch (aircraft->phase) {
        case BOARDING:
            if (aircraft->type == INTERNATIONAL)
                unlock_boarding_international();
            else
                unlock_boarding_domestic();
            break;
        case LANDING:
            if (aircraft->type == INTERNATIONAL)
                unlock_landing_international();
            else
                unlock_landing_domestic();
            break;
        case DEBOARDING:
            if (aircraft->type == INTERNATIONAL)
                unlock_deboarding_international();
            else
                unlock_deboarding_domestic();
            break;
        case TAKEOFF:
            if (aircraft->type == INTERNATIONAL)
                unlock_takeoff_international();
            else
                unlock_takeoff_domestic();
            break;
        default:
            break;
    }
}