#ifndef NETWORK_H
#define NETWORK_H

#define ROUTERS 5
#define INFINITY_COST 100000000
#define FINAL_B_D_COST 2

enum Router { A = 0, B, C, D, E };

typedef struct {
    int cost[ROUTERS][ROUTERS];
} Network;

extern const char ROUTER_LABELS[ROUTERS];

/* Inicializa uma rede nao direcionada com cinco roteadores. */
void network_init(Network *net);
/* Configura as duas direcoes de um enlace. */
void network_set_link(Network *net, int a, int b, int cost);
/* Apresenta os enlaces e o numero de vizinhos de cada roteador. */
void network_print(const Network *net);
/* Tabela: origem, destino, custo e primeiro roteador atraves do qual enviar. */
void network_print_routes(int source,
                          const int distances[ROUTERS],
                          const int next_hops[ROUTERS]);

#endif
