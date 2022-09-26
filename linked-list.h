// Feito por Guilherme Carbonari Boneti, GRR20196478, 02/2021.

#ifndef __LIST__
#define __LIST__

struct nodo
{
    struct ImageStruct *imagem;
    struct nodo *prox;
    struct nodo *ant;
};

struct lista_dupla_ligada
{
    struct nodo *ini;
    struct nodo *fim;
};

struct nodo *criaNodo(struct ImageStruct *img);

struct lista_dupla_ligada *inicializaLista();

void insereLista (struct lista_dupla_ligada *L, struct ImageStruct *img);

#endif