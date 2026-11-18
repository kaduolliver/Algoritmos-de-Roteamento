/*
 * Questao 5 - Dijkstra aplicado ao estado de enlace.
 *
 * Considera que o roteador conhece a topologia completa da rede.
 * Para cada roteador de origem, escolhe repetidamente o menor custo
 * tentativo e relaxa os enlaces adjacentes. Custos nao negativos.
 * Apresenta as iteracoes com origem A e as tabelas de TODOS os roteadores.
 */
#include "network.h"
#include <stdio.h>

static void print_trace(int step, int chosen,
                        const int dist[ROUTERS], const int done[ROUTERS]) {
    int j;
    printf("  Passo %d: fixa %c | distancias: ", step, ROUTER_LABELS[chosen]);
    for (j = 0; j < ROUTERS; ++j) {
        if (dist[j] >= INFINITY_COST) {
            printf("%c=INF%s ", ROUTER_LABELS[j], done[j] ? "*" : "");
        } else {
            printf("%c=%d%s ", ROUTER_LABELS[j], dist[j], done[j] ? "*" : "");
        }
    }
    printf("(* = custo definitivo)\n");
}

static void shortest_paths(const Network *net, int source,
                           int dist[ROUTERS], int previous[ROUTERS],
                           int next_hop[ROUTERS], int trace) {
    int done[ROUTERS] = {0};
    int i, j, chosen, min_cost, candidate, node, first;

    for (i = 0; i < ROUTERS; ++i) {
        dist[i] = INFINITY_COST;
        previous[i] = -1;
        next_hop[i] = -1;
    }
    dist[source] = 0;

    for (i = 0; i < ROUTERS; ++i) {
        min_cost = INFINITY_COST;
        chosen = -1;
        for (j = 0; j < ROUTERS; ++j) {
            if (!done[j] && dist[j] < min_cost) {
                min_cost = dist[j];
                chosen = j;
            }
        }
        if (chosen == -1) {
            break; /* Componentes inacessiveis, se existirem. */
        }
        done[chosen] = 1;

        for (j = 0; j < ROUTERS; ++j) {
            if (done[j] || net->cost[chosen][j] >= INFINITY_COST) {
                continue;
            }
            candidate = dist[chosen] + net->cost[chosen][j];
            if (candidate < dist[j]) {
                dist[j] = candidate;
                previous[j] = chosen;
            }
        }
        if (trace) {
            print_trace(i + 1, chosen, dist, done);
        }
    }

    /* Recupera o primeiro salto seguindo predecessores do destino ate a origem. */
    for (j = 0; j < ROUTERS; ++j) {
        if (j == source || dist[j] >= INFINITY_COST) {
            continue;
        }
        node = j;
        first = j;
        while (previous[node] != -1) {
            first = node;
            node = previous[node];
        }
        if (node == source) {
            next_hop[j] = first;
        }
    }
}

static void run_scenario(const Network *net, const char *name) {
    int source;
    int dist[ROUTERS], previous[ROUTERS], next_hop[ROUTERS];
    printf("\n=== CENARIO %s: DIJKSTRA ===\n", name);
    network_print(net);
    for (source = 0; source < ROUTERS; ++source) {
        if (source == A) {
            printf("\nIteracoes de Dijkstra com origem A:\n");
        }
        shortest_paths(net, source, dist, previous, next_hop, source == A);
        network_print_routes(source, dist, next_hop);
    }
}

int main(void) {
    Network net;
    network_init(&net);
    run_scenario(&net, "INICIAL");

    printf("\nALTERACAO: custo B-D de 5 para %d.\n", FINAL_B_D_COST);
    network_set_link(&net, B, D, FINAL_B_D_COST);
    run_scenario(&net, "FINAL");
    return 0;
}
