#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "simulation.c"

int main() {
    int num_runways;
    int num_gates;
    int num_towers;
    int time;

    printf("=== Configuração da Simulação do Aeroporto ===\n\n");

    printf("Quantas pistas o aeroporto terá? ");
    scanf("%d", &num_runways);

    printf("Quantos portões de embarque/desembarque o aeroporto terá? ");
    scanf("%d", &num_gates);

    printf("Quantas torres de controle o aeroporto terá? ");
    scanf("%d", &num_towers);

    printf("Qual será o tempo de simulação (em segundos)? ");
    scanf("%d", &time);

    printf("\n=== Iniciando simulação ===\n");
    start(&num_runways, &num_gates, &num_towers);
    
    printf("\n=== Simulação iniciada ===\n");
    for (int t = 1; t <= time; t++) {
        printf("Tempo: %d segundo(s)\n", t);

        sleep(1);
    }

    printf("\n=== Simulação finalizada ===\n");

    printf("\n=== Parâmetros da Simulação ===\n");
    printf("Pistas: %d\n", num_runways);
    printf("Portões: %d\n", num_gates);
    printf("Torres de Controle: %d\n", num_towers);
    printf("Tempo de Simulação: %d segundos\n", time);

    return 0;
}
