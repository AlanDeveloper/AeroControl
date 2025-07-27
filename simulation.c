#include <stdio.h>
#include "include/simulation.h"
#include "include/airport.h"
#include "include/aircraft.h"

extern volatile bool simulation_running;

void start(int *num_runways, int *num_gates, int *num_towers, int *operations_by_tower, int *num_domestic_aircraft, int *num_international_aircraft, pthread_t *aircraft_threads) {
    Airport airport;

    airport.num_runways = *num_runways;
    airport.num_gates = *num_gates;
    airport.num_towers = *num_towers;
    airport.num_domestic_aircraft = *num_domestic_aircraft;
    airport.num_international_aircraft = *num_international_aircraft;

    airport.runways = malloc(sizeof(Runway) * airport.num_runways);
    airport.gates = malloc(sizeof(Gate) * airport.num_gates);
    airport.towers = malloc(sizeof(ControlTower) * airport.num_towers);
    airport.domestic_aircrafts = malloc(sizeof(Aircraft*) * airport.num_domestic_aircraft);
    airport.international_aircrafts = malloc(sizeof(Aircraft*) * airport.num_international_aircraft);

    if (!airport.runways || !airport.gates || !airport.towers || !airport.domestic_aircrafts || !airport.international_aircrafts) {
        printf("Erro: falha na alocação de memória.\n");
        exit(1);
    }

    for (int i = 0; i < airport.num_runways; i++) {
        airport.runways[i].in_use = false;
        airport.runways[i].by_aircraft[0] = '\0';
    }

    for (int i = 0; i < airport.num_gates; i++) {
        airport.gates[i].in_use = false;
        airport.gates[i].by_aircraft[0] = '\0';
    }

    for (int i = 0; i < airport.num_towers; i++) {
        airport.towers[i].free_operates = *operations_by_tower;
    }

    for (int i = 0; i < airport.num_domestic_aircraft; i++) {
        airport.domestic_aircrafts[i] = create_aircraft(DOMESTIC);
        pthread_create(&aircraft_threads[i], NULL, aircraft_thread_function, (void*) airport.domestic_aircrafts[i]);
    }

    for (int i = 0; i < airport.num_international_aircraft; i++) {
        airport.international_aircrafts[i] = create_aircraft(INTERNATIONAL);
        pthread_create(&aircraft_threads[i], NULL, aircraft_thread_function, (void*) airport.international_aircrafts[i]);
    }

    printf("Aeroporto inicializado com sucesso:\n");
    printf("Pistas: %d\n", airport.num_runways);
    printf("Portões: %d\n", airport.num_gates);
    printf("Torres de Controle: %d com %d operações por torre\n", airport.num_towers, *operations_by_tower);
    printf("Aviões domésticos: %d\n", airport.num_domestic_aircraft);
    printf("Aviões internacionais: %d\n", airport.num_international_aircraft);
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

void* aircraft_thread_function(void* arg) {
    while (simulation_running) {
        usleep(100000);
    }
    return NULL;
}