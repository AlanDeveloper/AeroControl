#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "include/simulation.h"
#include "include/airport.h"

volatile bool simulation_running = true;
volatile int simulation_time = 0;

int main() {
    Airport airport;

    printf("=== Configuração da Simulação do Aeroporto ===\n\n");

    printf("Quantas pistas o aeroporto terá? ");
    scanf("%d", &airport.num_runways);

    printf("Quantos portões de embarque/desembarque o aeroporto terá? ");
    scanf("%d", &airport.num_gates);

    printf("Quantas torres de controle o aeroporto terá? ");
    scanf("%d", &airport.num_towers);

    printf("Quantas operações por torre? ");
    scanf("%d", &airport.operations_by_tower);

    printf("Quantos aviões domésticos? ");
    scanf("%d", &airport.num_domestic_aircraft);

    printf("Quantos aviões internacionais? ");
    scanf("%d", &airport.num_international_aircraft);

    printf("Qual será o tempo de simulação (em segundos)? ");
    scanf("%d", &simulation_time);

    printf("\n=== Iniciando simulação ===\n\n");
    int total_aircrafts = airport.num_domestic_aircraft + airport.num_international_aircraft;
    pthread_t *aircraft_threads = malloc(sizeof(pthread_t) * total_aircrafts);

    if (!aircraft_threads) {
        printf("Erro na alocação de memória para threads\n");
        exit(1);
    }
    start(&airport, aircraft_threads);
    
    printf("\n=== Simulação iniciada ===\n\n");
    for (int t = 1; t <= simulation_time; t++) {
        printf("Tempo: %02d segundo(s)\n", t);
        sleep(1);
    }
    simulation_running = false;

    printf("\n=== Simulação finalizada ===\n\n");

    for (int i = 0; i < total_aircrafts; i++) {
        pthread_join(aircraft_threads[i], NULL);
        printf("✓ Thread %d finalizada\n", i);
    }

    printf("\n=== Parâmetros da Simulação ===\n\n");
    printf("Pistas: %d\n", airport.num_runways);
    printf("Portões: %d\n", airport.num_gates);
    printf("Torres de Controle: %d\n", airport.num_towers);
    printf("Tempo de simulação: %02d segundo(s)\n", simulation_time);

    return 0;
}
