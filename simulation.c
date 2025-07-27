#include <stdio.h>
#include "include/simulation.h"
#include "include/airport.h"

void start(int *num_runways, int *num_gates, int *num_towers, int *operations_by_tower) {
    Airport airport;

    airport.num_runways = *num_runways;
    airport.num_gates = *num_gates;
    airport.num_towers = *num_towers;

    airport.runways = malloc(sizeof(Runway) * airport.num_runways);
    airport.gates = malloc(sizeof(Gate) * airport.num_gates);
    airport.towers = malloc(sizeof(ControlTower) * airport.num_towers);

    if (!airport.runways || !airport.gates || !airport.towers) {
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

    printf("Aeroporto inicializado com sucesso:\n");
    printf("Pistas: %d\n", airport.num_runways);
    printf("Portões: %d\n", airport.num_gates);
    printf("Torres de Controle: %d com %d operações por torre\n", airport.num_towers, *operations_by_tower);
}
