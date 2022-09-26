// Feito por Guilherme Carbonari Boneti, GRR20196478, 02/2021.
#include <stdlib.h>
#include "linked-list.h"

struct nodo *criaNodo(struct ImageStruct *img)
{
    struct nodo *aux;
    aux = malloc(sizeof(struct nodo));
    aux->imagem = img;
    aux->prox = NULL;
    aux->ant = NULL;
    return aux;
}

struct lista_dupla_ligada *inicializaLista() 
{
    struct lista_dupla_ligada *Laux;
    Laux = malloc(sizeof(struct lista_dupla_ligada));
    Laux->ini = NULL;
    Laux->fim = NULL;
    return Laux;
}

void insereLista (struct lista_dupla_ligada *L, struct ImageStruct *img)
{
    struct nodo *aux = criaNodo(img);
    if (L->ini == NULL)
    {
        L->ini = aux;
        L->fim = aux;
    }
    else
    {
        aux->ant = L->fim;
        L->fim->prox = aux;
        L->fim = aux;
    }
}