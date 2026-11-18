#include "network.h"
#include <stdio.h>

const char ROUTER_LABELS[ROUTERS] = {'A', 'B', 'C', 'D', 'E'};

void network_set_link(Network *net, int a, int b, int cost) {
    if (a < 0 || a >= ROUTERS || b < 0 || b >= ROUTERS || a == b || cost <= 0) {
        return; /* A rede pedagogica usa custos estritamente positivos. */
    }
    net->cost[a][b] = cost;
    net->cost[b][a] = cost;
}

void network_init(Network *net) {
    int i, j;
    for (i = 0; i < ROUTERS; ++i) {
        for (j = 0; j < ROUTERS; ++j) {
            net->cost[i][j] = (i == j) ? 0 : INFINITY_COST;
        }
    }

    /* Os enlaces sao bidirecionais; todos os roteadores possuem >=2 vizinhos. */
    network_set_link(net, A, B, 4);
    network_set_link(net, A, C, 2);
    network_set_link(net, B, C, 1);
    network_set_link(net, B, D, 5);
    network_set_link(net, C, D, 8);
    network_set_link(net, C, E, 10);
    network_set_link(net, D, E, 2);
}

void network_print(const Network *net) {
    int i, j, count;
    printf("Enlaces (bidirecionais):\n");
    for (i = 0; i < ROUTERS; ++i) {
        for (j = i + 1; j < ROUTERS; ++j) {
            if (net->cost[i][j] != INFINITY_COST) {
                printf("  %c -- %c : %d\n", ROUTER_LABELS[i],
                       ROUTER_LABELS[j], net->cost[i][j]);
            }
        }
    }
    printf("Conexoes diretas por roteador:\n");
    for (i = 0; i < ROUTERS; ++i) {
        count = 0;
        printf("  %c: ", ROUTER_LABELS[i]);
        for (j = 0; j < ROUTERS; ++j) {
            if (i != j && net->cost[i][j] < INFINITY_COST) {
                printf("%c ", ROUTER_LABELS[j]);
                ++count;
            }
        }
        printf("(%d interfaces/enlaces)\n", count);
    }
}

void network_print_routes(int source,
                          const int distances[ROUTERS],
                          const int next_hops[ROUTERS]) {
    int dest;
    printf("\nTabela do roteador %c:\n", ROUTER_LABELS[source]);
    printf("Origem -> Destino | Custo | Proximo salto\n");
    for (dest = 0; dest < ROUTERS; ++dest) {
        if (source == dest) {
            printf("%c -> %c | %5d | local\n", ROUTER_LABELS[source],
                   ROUTER_LABELS[dest], 0);
        } else if (distances[dest] >= INFINITY_COST) {
            printf("%c -> %c |   INF | inacessivel\n", ROUTER_LABELS[source],
                   ROUTER_LABELS[dest]);
        } else {
            printf("%c -> %c | %5d | %c\n", ROUTER_LABELS[source],
                   ROUTER_LABELS[dest], distances[dest],
                   ROUTER_LABELS[next_hops[dest]]);
        }
    }
}
