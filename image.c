// Feito por Guilherme Carbonari Boneti, GRR20196478, 02/2021.

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>
#include "image.h"

#define CREATOR "GUILHERME CARBONARI BONETI"
#define RGB_COMPONENT_COLOR 255

struct ImageStruct *readPPM(FILE *img_file)
{
    char formato[16];
    struct ImageStruct *img;
    int c, rgb;

    //read image format
    if (!fgets(formato, sizeof(formato), img_file)) {
        fprintf(stderr, "Erro ao ler formato da imagem'\n");
        exit(1);
    }

    //check the image format
    if (formato[0] != 'P' || !(formato[1] == '6' || formato[1] == '3')) {
        fprintf(stderr, "Formato de imagem inválido, deve ser 'P3 ou P6'\n");
        exit(1);
    }

    //alloc memory for image
    img = (struct ImageStruct *)malloc(sizeof(struct ImageStruct));
    if (!img) {
        fprintf(stderr, "Erro ao alocar memoria para a imagem\n");
        exit(1);
    }
    
    if (formato[1] == '6') {
        img->P3=0;
        img->P6=1;
    }
    else if (formato[1] == '3') {
        img->P3=1;
        img->P6=0;
    }

    //check for comments
    c = getc(img_file);
    while (c == '#') {
        while (getc(img_file) != '\n') ;
        c = getc(img_file);
    }
    ungetc(c, img_file);
    
    //read image size information
    if (fscanf(img_file, "%d %d", &img->width, &img->height) != 2) {
        fprintf(stderr, "Tamanho de imagem invalido (erro ao carregar imagem)\n");
        exit(1);
    }

    //read rgb component
    if (fscanf(img_file, "%d", &rgb) != 1) {
        fprintf(stderr, "componente rgb invalido (erro ao carregar imagem)\n");
        exit(1);
    }

    //check rgb component depth
    if (rgb!= RGB_COMPONENT_COLOR) {
        fprintf(stderr, "Os componentes da imagem nao sao representados por 1 byte\n");
        exit(1);
    }

    while (fgetc(img_file) != '\n') ;
    //memory allocation for pixel data
    img->data = (struct PixelStruct*)malloc(img->width * img->height * sizeof(struct PixelStruct));

    if (!img) {
        fprintf(stderr, "Erro ao alocar memoria para a imagem\n");
        exit(1);
    }

    //read pixel data from file
    if (img->P6) {
        if (fread(img->data, 3* img->width, img->height, img_file) != img->height) {
            fprintf(stderr, "Erro ao carregar imagem\n");
            exit(1);
        }
    }
    else if (img->P3) {
        for (int i=0; i < img->height*img->width; i++) {
            if (fscanf(img_file, "%hhd %hhd %hhd", &img->data[i].red, &img->data[i].green, &img->data[i].blue) < 3) {
                fprintf(stderr, "Erro ao carregar imagem\n");
                exit(1);
            }
        }
    }

    return img;
}
void writePPM(FILE *img_file, struct ImageStruct *img)
{
    //image format
    if (img->P6)
        fprintf(img_file, "P6\n");
    else if (img->P3)
        fprintf(img_file, "P3\n");

    //comments
    fprintf(img_file, "# Criado por %s\n", CREATOR);

    //image size
    fprintf(img_file, "%d %d\n",img->width,img->height);

    // rgb component depth
    fprintf(img_file, "%d\n", RGB_COMPONENT_COLOR);

    // pixel data
    if (img->P6) {
        fwrite(img->data, 3 * img->width, img->height, img_file);
    }
    else if (img->P3) {
        for (int i=0; i < img->height*img->width; i++) {
            fprintf(img_file, "%d %d %d ", img->data[i].red, img->data[i].green, img->data[i].blue);
        }
    }
}

void getAverageColor(struct ImageStruct *img)
{
    int i;
    unsigned int sum=0;
    if(img){
        for(i=0;i<img->width*img->height;i++){
            sum = sum + img->data[i].red;
        }
        img->averageColor.red = sum/(i+1);
        sum=0;
        for(i=0;i<img->width*img->height;i++){
            sum = sum + img->data[i].green;
        }
        img->averageColor.green = sum/(i+1);
        sum=0;
        for(i=0;i<img->width*img->height;i++){
            sum = sum + img->data[i].blue;
        }
        img->averageColor.blue = sum/(i+1);
    }
}

void atribuiBlocos(struct lista_dupla_ligada *L, struct ImageStruct *img, int blocoWidth, int blocoHeight) {
    
    for (int x=0; x<img->width*img->height; x+=blocoHeight*img->width) {       // percorre o vetor data de img
        for (int i=0; i<img->width; i+=blocoWidth) {       // percorre até imgWidth(1 linha) e pra cada blocoWidth cria um nodo e atribui os valores
            struct ImageStruct *bloco;
            bloco = (struct ImageStruct *)malloc(sizeof(struct ImageStruct));
            bloco->width = blocoWidth;
            bloco->height = blocoHeight;
            bloco->data = (struct PixelStruct*)malloc(bloco->width * bloco->height * sizeof(struct PixelStruct));
            for (int j=0; j<blocoWidth; j++) {
                bloco->data[j].red = img->data[i+j+x].red;
                bloco->data[j].green = img->data[i+j+x].green;
                bloco->data[j].blue = img->data[i+j+x].blue;
            }
            insereLista(L, bloco);        
        }
    }

    // repete o procedimento acima imgHeight vezes, passando por todas as linhas de um bloco, utilizando os nodos já criados
    for (int z=img->width, y=1; z<blocoHeight*img->width; z+=img->width, y++) {
        struct nodo *aux = L->ini;
        for (int x=z; x<img->width*img->height; x+=blocoHeight*img->width) {
            for (int i=x; i<x+img->width; i+=blocoWidth) {
                for (int j=0; j<blocoWidth; j++) {
                    aux->imagem->data[j+(blocoWidth*y)].red = img->data[i+j].red;
                    aux->imagem->data[j+(blocoWidth*y)].green = img->data[i+j].green;
                    aux->imagem->data[j+(blocoWidth*y)].blue = img->data[i+j].blue;
                }
                aux = aux->prox;     
            }
        }
    }

    struct nodo *aux = L->ini;
    for (; aux != NULL ;) {       // calcula a cor media de cada bloco
        getAverageColor(aux->imagem);
        aux = aux->prox;
    }
}

double distanciaCores(struct PixelStruct blocoColor, struct PixelStruct imgColor) {
    return sqrt(pow(blocoColor.red - imgColor.red, 2) + pow(blocoColor.green - imgColor.green, 2) + pow(blocoColor.blue - imgColor.blue, 2));
}

void trocaBlocosPastilhas (struct lista_dupla_ligada *listaBlocos, struct lista_dupla_ligada *listaImagens) {
    struct nodo *auxBlocos = listaBlocos->ini;
    struct nodo *auxImagens;
    struct nodo *imagemProxima;
    for (int i=0; auxBlocos != NULL ;i++) {
        imagemProxima = listaImagens->ini;
        auxImagens = listaImagens->ini;
        for (; auxImagens != NULL ;) {
            if (distanciaCores(auxBlocos->imagem->averageColor, auxImagens->imagem->averageColor) < distanciaCores(auxBlocos->imagem->averageColor, imagemProxima->imagem->averageColor)) {
                imagemProxima = auxImagens;
            }
            auxImagens = auxImagens->prox;
        }
        for (int i=0; i<auxBlocos->imagem->width*auxBlocos->imagem->height; i++) {
            auxBlocos->imagem->data[i].blue = imagemProxima->imagem->data[i].blue;
            auxBlocos->imagem->data[i].red = imagemProxima->imagem->data[i].red;
            auxBlocos->imagem->data[i].green = imagemProxima->imagem->data[i].green;
        }
        auxBlocos = auxBlocos->prox;
    }
}

void criaMosaico(struct lista_dupla_ligada *listaBlocos, struct ImageStruct *img, FILE *output) {
    int larguraBloco = listaBlocos->ini->imagem->width;
    int alturaBloco = listaBlocos->ini->imagem->height;
    struct ImageStruct *mosaico;
    mosaico = (struct ImageStruct *)malloc(sizeof(struct ImageStruct));
    mosaico->width = img->width;
    mosaico->height = img->height;
    mosaico->P3 = img->P3;
    mosaico->P6 = img->P6;
    mosaico->data = (struct PixelStruct*)malloc(mosaico->width * mosaico->height * sizeof(struct PixelStruct));
    for (int z=0, y=0; z<alturaBloco*img->width; z+=img->width, y++) {
        struct nodo *aux = listaBlocos->ini;
        for (int x=z; x<img->width*img->height; x+=alturaBloco*img->width) {
            for (int i=x; i<x+img->width; i+=larguraBloco) {
                for (int j=0; j<larguraBloco; j++) {
                    mosaico->data[i+j].red = aux->imagem->data[j+(larguraBloco*y)].red;
                    mosaico->data[i+j].green = aux->imagem->data[j+(larguraBloco*y)].green;
                    mosaico->data[i+j].blue = aux->imagem->data[j+(larguraBloco*y)].blue;
                }
                aux = aux->prox;
            }
        }
    }
    writePPM(output, mosaico);
}