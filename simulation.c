#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "include/simulation.h"
#include "include/resource_manager.h"
#include "include/airport.h"

extern volatile bool simulation_running;
extern volatile int simulation_time;
extern volatile int simulation_time_max;

void allocate_resources(Airport *airport) {
    airport->runways = malloc(sizeof(Runway) * airport->num_runways);
    airport->gates = malloc(sizeof(Gate) * airport->num_gates);
    airport->towers = malloc(sizeof(ControlTower) * airport->num_towers);
    airport->domestic_aircrafts = malloc(sizeof(Aircraft*) * airport->num_domestic_aircraft);
    airport->international_aircrafts = malloc(sizeof(Aircraft*) * airport->num_international_aircraft);

    if (!airport->runways || !airport->gates || !airport->towers || !airport->domestic_aircrafts || !airport->international_aircrafts) {
        printf("Erro: falha na alocação de memória.\n\n");
        exit(1);
    }

    init_resources(airport->num_runways, airport->num_gates, airport->num_towers, airport->operations_by_tower);
}

void initialize_resources(Airport *airport) {
    for (int i = 0; i < airport->num_runways; i++) {
        airport->runways[i].in_use = false;
        airport->runways[i].by_aircraft[0] = '\0';
    }

    for (int i = 0; i < airport->num_gates; i++) {
        airport->gates[i].in_use = false;
        airport->gates[i].by_aircraft[0] = '\0';
    }

    for (int i = 0; i < airport->num_towers; i++) {
        airport->towers[i].free_operates = airport->operations_by_tower;
    }
}

Aircraft* create_aircraft(FlightType type) {
    static int domestic_counter = 1;
    static int international_counter = 1;

    Aircraft* new_aircraft = malloc(sizeof(Aircraft));
    if (!new_aircraft) {
        printf("Erro: falha na alocação de memória para avião.\n");
        exit(1);
    }

    if (type == DOMESTIC) {
        snprintf(new_aircraft->id, sizeof(new_aircraft->id), "D%03d", domestic_counter++);
    } else {
        snprintf(new_aircraft->id, sizeof(new_aircraft->id), "I%03d", international_counter++);
    }

    new_aircraft->type = type;
    new_aircraft->phase = PHASE_NONE;

    return new_aircraft;
}

const char* get_phase_label_pt(FlightPhase phase) {
    switch (phase) {
        case PHASE_NONE:     return "nenhuma fase";
        case BOARDING:       return "embarque";
        case TAKEOFF:        return "decolagem";
        case FLY:            return "voo";
        case LANDING:        return "pouso";
        case DEBOARDING:     return "desembarque";
        default:             return "fase desconhecida";
    }
}

int decide_next_phase(Aircraft* aircraft) {
    switch (aircraft->phase) {
        case PHASE_NONE:     aircraft->phase = BOARDING; break;
        case BOARDING:       aircraft->phase = TAKEOFF; break;
        case TAKEOFF:        aircraft->phase = FLY; break;
        case FLY:            aircraft->phase = LANDING; break;
        case LANDING:        aircraft->phase = DEBOARDING; break;
        case DEBOARDING:     aircraft->phase = PHASE_NONE; break;
        default:             aircraft->phase = PHASE_NONE; break;
    }
    int seconds = (random() % 15) + 1;

    return seconds;
}

void perform_phase(Aircraft *aircraft, int seconds) {
    printf("[EVENT] Avião %-5s ► Entrou na fase %-12s │ Duração: %2d s\n",
       aircraft->id,
       get_phase_label_pt(aircraft->phase),
       seconds);
    for (int i = 0; i < seconds && simulation_running; i++) {
        sleep(1);
    }

    if (simulation_running) {
        printf("[EVENT] Avião %-5s ◄ Terminou a fase %-12s\n",
           aircraft->id,
           get_phase_label_pt(aircraft->phase));
    }
}

void* aircraft_thread_function(void* arg) {
    Aircraft* aircraft = (Aircraft*) arg;

    int seconds;
    while (simulation_running) {
        seconds = decide_next_phase(aircraft);

        lock_resources(aircraft);
        perform_phase(aircraft, seconds);
        unlock_resources(aircraft);
    }
    return NULL;
}

void create_and_start_aircraft_threads(Airport *airport, pthread_t *aircraft_threads) {
    int thread_index = 0;

    for (int i = 0; i < airport->num_domestic_aircraft; i++) {
        airport->domestic_aircrafts[i] = create_aircraft(DOMESTIC);
        pthread_create(&aircraft_threads[thread_index++], NULL, aircraft_thread_function, (void*) airport->domestic_aircrafts[i]);
    }

    for (int i = 0; i < airport->num_international_aircraft; i++) {
        airport->international_aircrafts[i] = create_aircraft(INTERNATIONAL);
        pthread_create(&aircraft_threads[thread_index++], NULL, aircraft_thread_function, (void*) airport->international_aircrafts[i]);
    }
}

void print_airport_summary(Airport *airport) {
    printf("Aeroporto inicializado com sucesso.\n\n");
    printf("Pistas: %d\n", airport->num_runways);
    printf("Portões: %d\n", airport->num_gates);
    printf("Torres de Controle: %d com %d operações por torre\n", airport->num_towers, airport->operations_by_tower);
    printf("Aviões domésticos: %d\n", airport->num_domestic_aircraft);
    printf("Aviões internacionais: %d\n", airport->num_international_aircraft);
}

void start(Airport *airport, pthread_t *aircraft_threads) {
    allocate_resources(airport);
    initialize_resources(airport);
    print_airport_summary(airport);
}