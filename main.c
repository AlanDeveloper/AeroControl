#include <stdio.h>
#include <stdlib.h>

#include "simulation.c"

int main() {
    int num_pistas;
    int num_portoes;
    int num_torres;
    int tempo_simulacao;

    printf("=== Configuração da Simulação do Aeroporto ===\n");

    printf("Quantas pistas o aeroporto terá? ");
    scanf("%d", &num_pistas);

    printf("Quantos portões de embarque/desembarque o aeroporto terá? ");
    scanf("%d", &num_portoes);

    printf("Quantas torres de controle o aeroporto terá? ");
    scanf("%d", &num_torres);

    printf("Qual será o tempo de simulação (em segundos)? ");
    scanf("%d", &tempo_simulacao);

    printf("=== Iniciando simulação ===");
    start(&num_pistas, &num_portoes, &num_torres, &tempo_simulacao);

    printf("\n--- Parâmetros da Simulação ---\n");
    printf("Pistas: %d\n", num_pistas);
    printf("Portões: %d\n", num_portoes);
    printf("Torres de Controle: %d\n", num_torres);
    printf("Tempo de Simulação: %d segundos\n", tempo_simulacao);

    return 0;
}
