#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>
#include "grafo.h"
#include "vertice.h"
#include "lista_enc.h"
#include "no.h"


#define FALSE 0
#define TRUE 1

//#define DEBUG
#define DEBUG_LARGURA
//#define DEBUG_PROFUNDIDADE

#define INFINITO INT_MAX

struct grafos {
	int id;
	lista_enc_t *vertices;
};

void print_vertice (grafo_t *grafo)
{
    no_t * no = obter_cabeca(grafo->vertices);

    while (no)
    {
        vertice_t * v = obter_dado(no);
        printf("dados do grafo: %s\n", vertice_get_name(v));
        no = obtem_proximo(no);
    }

}

grafo_t *cria_grafo(int id)
{
	grafo_t *p = NULL;

	p = (grafo_t*) malloc(sizeof(grafo_t));

	if (p == NULL)	{
		perror("cria_grafo:");
		exit(EXIT_FAILURE);
	}

	p->id = id;
	p->vertices = cria_lista_enc();

	return p;
}

vertice_t* grafo_adicionar_vertice(grafo_t *grafo, char *name)
{
	vertice_t *vertice;
	no_t *no;

	if (grafo == NULL)	{
			fprintf(stderr,"grafo_adicionar_vertice: grafo invalido!");
			exit(EXIT_FAILURE);
	}

	if (procura_vertice(grafo, name) != NULL) {
		fprintf(stderr,"grafo_adicionar_vertice: vertice duplicado!\n");
		exit(EXIT_FAILURE);
	}

	vertice = cria_vertice(name);
	no = cria_no(vertice);

	add_cauda(grafo->vertices, no);

	return vertice;
}

vertice_t* procura_vertice(grafo_t *grafo, char *name)
{
	no_t *no_lista;
	vertice_t *vertice;
	char *name_id;

	if (grafo == NULL)	{
		fprintf(stderr,"procura_vertice: grafo invalido!");
		exit(EXIT_FAILURE);
	}

	if (lista_vazia(grafo->vertices) == TRUE){
		return NULL;
	}

	no_lista = obter_cabeca(grafo->vertices);

	while (no_lista)
	{
		//obtem o endereco da lista
		vertice = obter_dado(no_lista);

		//obterm o id do vertice
		name_id = vertice_get_name(vertice);

		if (strcmp(name_id, name) == 0) {
            #ifdef DEBUG
            printf("name: %s\n", vertice_get_name(vertice));
            #endif // DEBUG_LARGURA
			return vertice;
		}

		no_lista = obtem_proximo(no_lista);
	}

	return NULL;
}

void adiciona_adjacentes(grafo_t *grafo, vertice_t *vertice_1, vertice_t *vertice_2, float peso)
{
	arestas_t *aresta;

    aresta = cria_aresta(vertice_1, vertice_2, peso);
    adiciona_aresta(vertice_1, aresta);
    aresta = cria_aresta(vertice_2, vertice_1, peso);
    adiciona_aresta(vertice_2, aresta);

    #ifdef DEBUG
        printf("\tvertice: %s\n", vertice_get_name(vertice_1));
        printf("\tsucessor: %s\n", vertice_get_name(vertice_2));
        printf("\tpeso: %f\n", peso);
    #endif

}


void exportar_grafo_dot(const char *filename, grafo_t *grafo)
{
	FILE *file;

	no_t *no_vert;
	no_t *no_arest;
	vertice_t *vertice;
	vertice_t *adjacente;
	arestas_t *aresta;
	arestas_t *contra_aresta;
	lista_enc_t *lista_arestas;

	float peso;

	if (filename == NULL || grafo == NULL){
		fprintf(stderr, "exportar_grafp_dot: ponteiros invalidos\n");
		exit(EXIT_FAILURE);
	}

	file = fopen(filename, "w");

	if (file == NULL){
		perror("exportar_grafp_dot:");
		exit(EXIT_FAILURE);
	}

	fprintf(file, "graph {\n");

	//obtem todos os nos da lista
	no_vert = obter_cabeca(grafo->vertices);
	//printf("cabeca_grafo :%s\n", vertice_get_name(obter_dado(no_vert)));
	while (no_vert){
		vertice = obter_dado(no_vert);

		//obtem todos as arestas
		lista_arestas = vertice_get_arestas(vertice);

		no_arest = obter_cabeca(lista_arestas);
		while (no_arest) {
			aresta = obter_dado(no_arest);
			//ignora caso já exportada
            if (aresta_get_status(aresta) == EXPORTADA) {
				no_arest = obtem_proximo(no_arest);
				continue;
			}

			//marca como exportada esta aresta
			aresta_set_status(aresta, EXPORTADA);
			adjacente = aresta_get_adjacente(aresta);

			//marca contra-aresta também como exporta no caso de grafo não direcionados
			contra_aresta = procurar_adjacente(adjacente, vertice);
			aresta_set_status(contra_aresta, EXPORTADA);

			//obtem peso
			peso = aresta_get_peso(aresta);

			fprintf(file, "\t%s -- %s [label = %f];\n",
					vertice_get_name(vertice),
					vertice_get_name(adjacente),
					peso);

			no_arest = obtem_proximo(no_arest);
		}
		no_vert = obtem_proximo(no_vert);
	}
	fprintf(file, "}\n");
	fclose(file);
}

/*
void libera_grafo (grafo_t *grafo){
	no_t *no_vert;
	no_t *no_arest;
	no_t *no_liberado;
	vertice_t *vertice;
	arestas_t *aresta;
	lista_enc_t *lista_arestas;

	if (grafo == NULL) {
		fprintf(stderr, "libera_grafo: grafo invalido\n");
		exit(EXIT_FAILURE);
	}

	//varre todos os vertices
	no_vert = obter_cabeca(grafo->vertices);
	while (no_vert){
		vertice = obter_dado(no_vert);

		//libera todas as arestas
		lista_arestas = vertice_get_arestas(vertice);
		no_arest = obter_cabeca(lista_arestas);
		while (no_arest){
			aresta = obter_dado(no_arest);

			//libera aresta
			free(aresta);

			//libera no da lsita
			no_liberado = no_arest;
			no_arest = obtem_proximo(no_arest);
			free(no_liberado);
		}

		//libera lista de arestas e vertice
		free(lista_arestas);
		free(vertice);

		//libera no da lista
		no_liberado = no_vert;
		no_vert = obtem_proximo(no_vert);
		free(no_liberado);
	}

	//libera grafo e vertice
	free(grafo->vertices);
	free(grafo);
}

lista_enc_t* lista_de_vertices(grafo_t*grafo)
{
    return grafo->vertices;
}*/
