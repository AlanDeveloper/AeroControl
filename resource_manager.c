#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "include/aircraft.h"
#include "include/flight.h"

extern volatile int crashed_planes;
extern volatile int successful_flights;
extern volatile int simulation_running;

static int total_runways = 0;
static int total_gates = 0;
static int total_towers = 0;
static int ops_per_tower = 0;

static int available_runways = 0;
static int available_gates = 0;
static int available_tower_ops = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int waiting_international = 0;
static int waiting_domestic = 0;
static int domestic_priority = 0;

static int seconds_since(struct timespec start) {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return (int)(now.tv_sec - start.tv_sec);
}

void init_resources(int runways, int gates, int towers, int max_ops_per_tower) {
    pthread_mutex_lock(&mutex);

    total_runways = runways;
    total_gates = gates;
    total_towers = towers;
    ops_per_tower = max_ops_per_tower;

    available_runways = runways;
    available_gates = gates;
    available_tower_ops = towers * max_ops_per_tower;

    waiting_international = 0;
    waiting_domestic = 0;
    domestic_priority = 0;

    pthread_mutex_unlock(&mutex);
}

void destroy_resources() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

static int can_acquire_resources(Aircraft* aircraft) {
    int resources_available = 0;
    
    if (aircraft->phase == LANDING) {
        resources_available = (available_runways > 0 && available_gates > 0 && available_tower_ops > 0);
    }
    else if (aircraft->phase == TAKEOFF) {
        resources_available = (available_runways > 0 && available_tower_ops > 0);
    }
    else if (aircraft->phase == BOARDING || aircraft->phase == DEBOARDING) {
        resources_available = (available_gates > 0);
    }
    else {
        resources_available = 1;
    }
    
    if (!resources_available) {
        return 0;
    }
    
    if (waiting_international > 0) {
        if (aircraft->type == INTERNATIONAL) {
            return 1;
        } else {
            return domestic_priority;
        }
    }
    
    return 1;
}

void increment_crashed_planes() {
    crashed_planes++;
}

void wake_all_threads() {
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}

void lock_resources(Aircraft* aircraft) {
    pthread_mutex_lock(&mutex);
    printf("[LOCK] %s tentando recursos para fase %s\n", aircraft->id, get_phase_label_pt(aircraft->phase));

    struct timespec wait_start;
    clock_gettime(CLOCK_REALTIME, &wait_start);

    if (aircraft->type == INTERNATIONAL)
        waiting_international++;
    else
        waiting_domestic++;

    while (!can_acquire_resources(aircraft)) {
        if (aircraft->type != INTERNATIONAL && aircraft->phase == LANDING) {
            int waited = seconds_since(wait_start);
            
            if (waited >= 90) {
                printf("AVISO: Avião doméstico %s caiu após esperar %d segundos!\n", aircraft->id, waited);
                
                waiting_domestic--;
                
                if (waiting_domestic == 0) {
                    domestic_priority = 0;
                }
                
                crashed_planes++;
                pthread_cond_broadcast(&cond);
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);
            }
            else if (waited >= 60 && domestic_priority == 0) {
                domestic_priority = 1;
                printf("ALERTA: Avião doméstico %s esperando para pousar há %d segundos! Ganhou prioridade!\n", aircraft->id, waited);
                pthread_cond_broadcast(&cond);
            }
        }

        printf("[WAIT] %s esperando. Recursos disponíveis: pistas=%d, portões=%d, torres=%d. Esperando: domésticos=%d, internacionais=%d, prioridade doméstica=%d\n",
            aircraft->id, available_runways, available_gates, available_tower_ops,
            waiting_domestic, waiting_international, domestic_priority);
        
        pthread_cond_wait(&cond, &mutex);
    }

    if (aircraft->phase == LANDING) {
        available_runways--;
        available_gates--;
        available_tower_ops--;
    }
    else if (aircraft->phase == TAKEOFF) {
        available_runways--;
        available_tower_ops--;
    }
    else if (aircraft->phase == BOARDING || aircraft->phase == DEBOARDING) {
        available_gates--;
    }

    if (aircraft->type == INTERNATIONAL)
        waiting_international--;
    else
        waiting_domestic--;

    if (waiting_domestic == 0) {
        domestic_priority = 0;
    }
    
    printf("[LOCK] %s conseguiu recursos\n", aircraft->id);
    pthread_mutex_unlock(&mutex);
}

void unlock_resources(Aircraft* aircraft) {
    printf("[UNLOCK] %s tentando liberar recursos da fase %s\n", aircraft->id, get_phase_label_pt(aircraft->phase));
    
    if (pthread_mutex_trylock(&mutex) != 0) {
        pthread_mutex_lock(&mutex);
    }

    if (aircraft->phase == LANDING) {
        available_runways++;
        available_gates++;
        available_tower_ops++;
    }
    else if (aircraft->phase == TAKEOFF) {
        available_runways++;
        available_tower_ops++;
    }
    else if (aircraft->phase == BOARDING || aircraft->phase == DEBOARDING) {
        available_gates++;

        if (aircraft->phase == DEBOARDING) {
            successful_flights++;
        }
    }
    
    printf("[UNLOCK] %s liberou recursos da fase %s. Disponível: pistas=%d, portões=%d, torres=%d\n", 
           aircraft->id, get_phase_label_pt(aircraft->phase), available_runways, available_gates, available_tower_ops);

    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
}