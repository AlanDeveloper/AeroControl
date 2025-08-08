#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "include/resource_manager.h"
#include "include/simulation.h"
#include "include/airport.h"

volatile bool simulation_running = true;
volatile int simulation_time_max = 0;
volatile int simulation_time = 1;

int get_positive_input(const char *prompt) {
    int value;
    do {
        printf("%s", prompt);
        scanf("%d", &value);
        if (value <= 0) {
            printf("Valor inválido. Digite um número maior que 0.\n");
        }
    } while (value <= 0);
    return value;
}

void setup_airport(Airport *airport) {
    printf("=== Configuração da Simulação do Aeroporto ===\n\n");
    
    airport->num_runways = get_positive_input("Quantas pistas o aeroporto terá? ");
    airport->num_gates = get_positive_input("Quantos portões de embarque/desembarque o aeroporto terá? ");
    airport->num_towers = get_positive_input("Quantas torres de controle o aeroporto terá? ");
    airport->operations_by_tower = get_positive_input("Quantas operações por torre? ");
    airport->num_domestic_aircraft = get_positive_input("Quantos aviões domésticos? ");
    airport->num_international_aircraft = get_positive_input("Quantos aviões internacionais? ");
    simulation_time_max = get_positive_input("Qual será o tempo de simulação (em segundos)? ");
}

void print_simulation_summary(const Airport *airport) {
    printf("\n=== Parâmetros da Simulação ===\n\n");
    printf("Pistas: %d\n", airport->num_runways);
    printf("Portões: %d\n", airport->num_gates);
    printf("Torres de Controle: %d\n", airport->num_towers);
    printf("Tempo de simulação: %02d segundo(s)\n", simulation_time_max);
}

int main() {
    Airport airport;
    setup_airport(&airport);
    
    printf("\n=== Iniciando simulação ===\n\n");
    int total_aircrafts = airport.num_domestic_aircraft + airport.num_international_aircraft;
    pthread_t *aircraft_threads = malloc(sizeof(pthread_t) * total_aircrafts);
    
    if (!aircraft_threads) {
        fprintf(stderr, "Erro na alocação de memória para threads\n");
        exit(EXIT_FAILURE);
    }
    
    start(&airport, aircraft_threads);
    
    printf("\n=== Simulação iniciada ===\n\n");
    create_and_start_aircraft_threads(&airport, aircraft_threads);
    for (int simulation_time = 1; simulation_time < simulation_time_max; simulation_time++) {
        printf("[TEMPO] %02d segundo(s) decorridos.\n", simulation_time);
        sleep(1);
    }
    simulation_running = false;
    
    printf("\n=== Simulação finalizada ===\n\n");
    
    for (int i = 0; i < total_aircrafts; i++) {
        pthread_join(aircraft_threads[i], NULL);
        printf("✓ Thread %d finalizada\n", i);
    }
    resource_manager_destroy();
    
    print_simulation_summary(&airport);
    
    for (int i = 0; i < airport.num_domestic_aircraft; i++) {
        free(airport.domestic_aircrafts[i]);
    }
    for (int i = 0; i < airport.num_international_aircraft; i++) {
        free(airport.international_aircrafts[i]);
    }
    
    free(airport.domestic_aircrafts);
    free(airport.international_aircrafts);
    
    free(airport.runways);
    free(airport.gates);
    free(airport.towers);
    
    free(aircraft_threads);
    
    return 0;
}