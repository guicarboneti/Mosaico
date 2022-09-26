// Feito por Guilherme Carbonari Boneti, GRR20196478, 02/2021.

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>
#include "image.h"

int main(int argc, char *argv[]){
 
    DIR *dirstream;
    struct dirent *direntry;
    struct ImageStruct *big_image;
    struct lista_dupla_ligada *listaImagens = inicializaLista();
    struct lista_dupla_ligada *listaBlocosBigImage = inicializaLista();
    char optc = 0;
    int ajuda = 0;
    char pastilhas[80] = "./tiles/";
    char entrada[80] = "\0";
    char saida[80] = "\0";

    // tratamento das opcoes de chamada
    while((optc = getopt (argc, argv, "hi:p:o:")) != -1) {
        switch(optc) {
            case 'i' :
                strcpy(entrada, optarg);
                break;
            case 'p' :
                strcpy(pastilhas, optarg);
                int i;
                for (i=0; pastilhas[i] != '\0'; i++);
                if (pastilhas[i-1] != '/') {
                    pastilhas[i] = '/';
                    pastilhas[i+1] = '\0';
                }
                break;
            case 'o' :
                strcpy(saida, optarg);
                break;
            case 'h' :
                ajuda = 1;
                break;
            default : // Qualquer parametro nao tratado
                fprintf(stderr, "O parametro esta incorreto.\n");
                exit(1);
        }
    }

    // gera uma mensagem de ajuda, explicando o que o programa faz e quais as opções disponíveis.
    if (ajuda == 1) {
        fprintf(stderr, "\nO programa cria um mosaico construido a partir de imagens pequenas (pastilhas).\nAs pastilhas são escolhidas de forma a reproduzir a cor e intensidade luminosa do bloco da imagem original correspondente, gerando um efeito visual muito interessante!\n");
        fprintf(stderr, "\nAs imagens fornecidas devem ser em formato PPM\n");
        fprintf(stderr, "As opcoes de chamada do programa sao:\n     -i: indica a imagem de entrada; se não for informada, assume-se a entrada padrão (stdin).\n     -o: indica a imagem de saída; se não for informada, assume-se a saída padrão (stdout).\n     -p: indica o diretório de pastilhas; se não for informado, assume-se o diretório './tiles'.\n     -h: gera esta mensagem de ajuda e encerra o programa.\n\n");
        exit(0);
    }

    // abre o diretório
    dirstream = opendir (pastilhas);
    if ( ! dirstream )
    {
        fprintf(stderr, "Nao foi possivel abrir o diretorio '%s'\n", pastilhas);
        exit (1) ;
    }

    // varre as entradas do diretório aberto
    fprintf(stderr, "Reading tiles from '%s'\n", pastilhas);
    fprintf(stderr, "Calculating tiles' average colors\n");
    int i;
    for (i=1;;i++)
    {
        // pega a próxima entrada
        direntry = readdir (dirstream) ;

        // se for nula, encerra a varredura
        if (! direntry)
            break ;

        if (strcmp(direntry->d_name, ".") != 0 && strcmp(direntry->d_name, "..") != 0) {

            char file_path[80] = "\0";
            strcpy(file_path, pastilhas);
            strcat(file_path, direntry->d_name);

            FILE *fp;
            struct ImageStruct *image;

            fp = fopen(file_path, "r");
                if (!fp) {
                    fprintf(stderr, "Nao foi possivel abrir o arquivo '%s'\n", file_path);
                    exit(1);
                }
                image = readPPM(fp);
            fclose(fp);

            getAverageColor(image);
            insereLista(listaImagens, image);
        }
    }
    fprintf(stderr, "%d tiles read\n", i);
    fprintf(stderr, "Tile size is %dx%d\n", listaImagens->ini->imagem->width, listaImagens->ini->imagem->height);    

    // fecha o diretório
    (void) closedir (dirstream);

    fprintf(stderr, "Reading input image\n");
    // le a imagem grande
    if (entrada[0] == '\0')
        big_image = readPPM(stdin);
    else {
        FILE *fp;
        fp = fopen(entrada, "r");
            if (!fp) {
                fprintf(stderr, "Nao foi possivel abrir o arquivo '%s'\n", entrada);
                exit(1);
            }
            big_image = readPPM(fp);
        fclose(fp);
    }

    if (big_image->P3)
        fprintf(stderr, "Input image is PPM P3, %dx%d pixels\n", big_image->width, big_image->height);
    else if (big_image->P6)
        fprintf(stderr, "Input image is PPM P6, %dx%d pixels\n", big_image->width, big_image->height);

    // divide a imagem grande em blocos, calculando suas cores medias
    atribuiBlocos(listaBlocosBigImage, big_image, listaImagens->ini->imagem->width, listaImagens->ini->imagem->height);

    // troca os blocos pelas pastilhas mais proximas
    fprintf(stderr, "Building mosaic image\n");
    trocaBlocosPastilhas(listaBlocosBigImage, listaImagens);

    fprintf(stderr, "Writing output file\n");
    // imprime os blocos trocados no arquivo especificado
    if (saida[0] == '\0') {
        criaMosaico(listaBlocosBigImage, big_image, stdout);
    }
    else {
        FILE *fp;
        fp = fopen(saida, "w");
        if (!fp) {
            fprintf(stderr, "Nao foi possivel abrir o arquivo '%s'\n", saida);
            exit(1);
        }
            criaMosaico(listaBlocosBigImage, big_image, fp);
        fclose(fp);
    }

    free(listaImagens);
    free(listaBlocosBigImage);
    free(big_image);

    exit (0);
}