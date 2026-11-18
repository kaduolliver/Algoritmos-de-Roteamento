/*
 * Questao 4 - Bellman-Ford aplicado ao roteamento por vetor de distancias.
 *
 * Cada roteador conhece inicialmente os seus enlaces diretos. Em cada
 * rodada SINCRONA, todos recebem as estimativas da rodada anterior dos
 * seus vizinhos e aplicam a equacao de Bellman-Ford:
 *     D_x(d) = min_{v vizinho de x} { c(x,v) + D_v(d) }
 *
 * Este e um simulador didatico, nao uma implementacao integral do RIP.
 */
#include "network.h"
#include <stdio.h>

typedef struct {
    int distance[ROUTERS][ROUTERS];
    int next_hop[ROUTERS][ROUTERS];
} DistanceVectors;

static void initialize_vectors(const Network *net, DistanceVectors *dv) {
    int src, dst;
    for (src = 0; src < ROUTERS; ++src) {
        for (dst = 0; dst < ROUTERS; ++dst) {
            dv->distance[src][dst] = net->cost[src][dst];
            dv->next_hop[src][dst] =
                (src != dst && net->cost[src][dst] < INFINITY_COST) ? dst : -1;
        }
    }
}

static void print_vectors(const DistanceVectors *dv, int round) {
    int src, dst;
    printf("\nRodada %d (custos conhecidos):\n", round);
    printf("     A    B    C    D    E\n");
    for (src = 0; src < ROUTERS; ++src) {
        printf("%c ", ROUTER_LABELS[src]);
        for (dst = 0; dst < ROUTERS; ++dst) {
            if (dv->distance[src][dst] >= INFINITY_COST) {
                printf(" %4s", "INF");
            } else {
                printf(" %4d", dv->distance[src][dst]);
            }
        }
        putchar('\n');
    }
}

static void run_scenario(const Network *net, const char *name) {
    DistanceVectors old, current;
    int round, src, dst, neighbor, best, hop, candidate, changed;

    printf("\n=== CENARIO %s: BELLMAN-FORD ===\n", name);
    network_print(net);
    initialize_vectors(net, &old);
    print_vectors(&old, 0);

    for (round = 1; round < ROUTERS; ++round) {
        changed = 0;
        /* Todas as atualizacoes consultam exclusivamente a rodada anterior. */
        for (src = 0; src < ROUTERS; ++src) {
            for (dst = 0; dst < ROUTERS; ++dst) {
                if (src == dst) {
                    current.distance[src][dst] = 0;
                    current.next_hop[src][dst] = -1;
                    continue;
                }
                best = INFINITY_COST;
                hop = -1;
                for (neighbor = 0; neighbor < ROUTERS; ++neighbor) {
                    if (src == neighbor || net->cost[src][neighbor] >= INFINITY_COST ||
                        old.distance[neighbor][dst] >= INFINITY_COST) {
                        continue;
                    }
                    candidate = net->cost[src][neighbor] + old.distance[neighbor][dst];
                    if (candidate < best) {
                        best = candidate;
                        hop = neighbor;
                    }
                }
                current.distance[src][dst] = best;
                current.next_hop[src][dst] = hop;
                if (best != old.distance[src][dst] || hop != old.next_hop[src][dst]) {
                    changed = 1;
                }
            }
        }
        old = current; /* Snapshot: a proxima rodada usa os valores novos. */
        print_vectors(&old, round);
        if (!changed) {
            printf("Sem alteracoes nesta rodada: tabelas estabilizadas.\n");
            break;
        }
    }

    printf("\nROTAS FINAIS (%s)\n", name);
    for (src = 0; src < ROUTERS; ++src) {
        network_print_routes(src, old.distance[src], old.next_hop[src]);
    }
}

int main(void) {
    Network net;
    network_init(&net);
    run_scenario(&net, "INICIAL");

    /* Simulacao de melhora no enlace B-D: de custo 5 para custo 2. */
    printf("\nALTERACAO: custo B-D de 5 para %d.\n", FINAL_B_D_COST);
    network_set_link(&net, B, D, FINAL_B_D_COST);
    run_scenario(&net, "FINAL");
    return 0;
}
