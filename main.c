#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "simulation.c"

volatile bool simulation_running = true;

int main() {
    int num_runways;
    int num_gates;
    int num_towers;
    int operations_by_tower;
    int num_domestic_aircraft;
    int num_international_aircraft;
    int time;

    printf("=== Configuração da Simulação do Aeroporto ===\n\n");

    printf("Quantas pistas o aeroporto terá? ");
    scanf("%d", &num_runways);

    printf("Quantos portões de embarque/desembarque o aeroporto terá? ");
    scanf("%d", &num_gates);

    printf("Quantas torres de controle o aeroporto terá? ");
    scanf("%d", &num_towers);

    printf("Quantas operações por torre? ");
    scanf("%d", &operations_by_tower);

    printf("Quantos aviões domésticos? ");
    scanf("%d", &num_domestic_aircraft);

    printf("Quantos aviões internacionais? ");
    scanf("%d", &num_international_aircraft);

    printf("Qual será o tempo de simulação (em segundos)? ");
    scanf("%d", &time);

    printf("\n=== Iniciando simulação ===\n\n");
    int total_aircrafts = num_domestic_aircraft + num_international_aircraft;
    pthread_t *aircraft_threads = malloc(sizeof(pthread_t) * total_aircrafts);

    if (!aircraft_threads) {
        printf("Erro na alocação de memória para threads\n");
        exit(1);
    }
    start(&num_runways, &num_gates, &num_towers, &operations_by_tower, &num_domestic_aircraft, &num_international_aircraft, aircraft_threads);
    
    printf("\n=== Simulação iniciada ===\n\n");
    for (int t = 1; t <= time; t++) {
        printf("Tempo: %d segundo(s)\n", t);
        sleep(1);
    }
    simulation_running = false;

    printf("\n=== Simulação finalizada ===\n\n");

    for (int i = 0; i < total_aircrafts; i++) {
        pthread_join(aircraft_threads[i], NULL);
        printf("Finalizando thread: %d\n", i);
    }

    printf("\n=== Parâmetros da Simulação ===\n\n");
    printf("Pistas: %d\n", num_runways);
    printf("Portões: %d\n", num_gates);
    printf("Torres de Controle: %d\n", num_towers);
    printf("Tempo de Simulação: %d segundos\n", time);

    return 0;
}
