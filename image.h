// Feito por Guilherme Carbonari Boneti, GRR20196478, 02/2021.

#ifndef __IMG__
#define __IMG__
#include <stdio.h>
#include "linked-list.h"

struct PixelStruct {
    unsigned char red,green,blue;
};

struct ImageStruct {
    int width, height, P3, P6;
    struct PixelStruct averageColor;
    struct PixelStruct *data;
};

struct ImageStruct *readPPM(FILE *img_file);

void writePPM(FILE *img_file, struct ImageStruct *img);

void getAverageColor(struct ImageStruct *img);

void atribuiBlocos(struct lista_dupla_ligada *L, struct ImageStruct *img, int blocoWidth, int blocoHeight);

double distanciaCores(struct PixelStruct blocoColor, struct PixelStruct imgColor);

void trocaBlocosPastilhas (struct lista_dupla_ligada *listaBlocos, struct lista_dupla_ligada *listaImagens);

void criaMosaico(struct lista_dupla_ligada *listaBlocos, struct ImageStruct *img, FILE *output);

#endif