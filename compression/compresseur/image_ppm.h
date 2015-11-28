// ----------------------------------------------------------------------------
// Filename        : image_ppm.c
// Description     :
// Created On      : Tue Mar 31 13:26:36 2005
// ----------------------------------------------------------------------------

#ifndef IMAGE_PPM
#define IMAGE_PPM

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>

#define RED 1
#define GREEN 2
#define BLUE 3

#define Yc 1
#define Cr 2
#define Cb 3

#define allocation_tableau(nom, type, nombre) \
if( (nom = (type*) calloc (nombre, sizeof(type) ) ) == NULL ) \
{\
 printf("\n Allocation dynamique impossible pour un pointeur-tableau \n");\
 exit(EXIT_FAILURE);\
}

typedef unsigned char OCTET;

void ignorer_commentaires(FILE * f);
void ecrire_image_ppm(char  nom_image[], OCTET *pt_image, int nb_lignes, int nb_colonnes);
void lire_nb_lignes_colonnes_image_ppm(char nom_image[], int *nb_lignes, int *nb_colonnes);
void lire_image_ppm(char  nom_image[], OCTET *pt_image, int taille_image);
void planR(OCTET *pt_image, OCTET *src, int taille_image);
void planV(OCTET *pt_image, OCTET *src, int taille_image);
void planB(OCTET *pt_image, OCTET *src, int taille_image);
void ecrire_image_pgm(char  nom_image[], OCTET *pt_image, int nb_lignes, int nb_colonnes);
void lire_nb_lignes_colonnes_image_pgm(char nom_image[], int *nb_lignes, int *nb_colonnes);
void lire_image_pgm(char  nom_image[], OCTET *pt_image, int taille_image);
OCTET& at(OCTET* array, int width, int height, int x, int y, int color);
#endif
