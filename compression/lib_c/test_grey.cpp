// test_couleur.cpp : Seuille une image en niveau de gris

#include <stdio.h>
#include "image_ppm.h"

int main(int argc, char* argv[])
{
	char cNomImgLue[250], cNomImgEcrite[250];
	int nH, nW, nTaille, S;

	if (argc != 4) {
		printf("Usage: ImageIn.pgm ImageOut.pgm Seuil \n"); 
		exit (1) ;
	}

	sscanf (argv[1],"%s",cNomImgLue) ;
	sscanf (argv[2],"%s",cNomImgEcrite);
	sscanf (argv[3],"%d",&S);

	int s1 = 50, s2 = 100, s3 = 150, s4 = 200;

	OCTET *ImgIn, *ImgOut;

	lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &nH, &nW);
	nTaille = nH * nW;

	allocation_tableau(ImgIn, OCTET, nTaille);
	lire_image_pgm(cNomImgLue, ImgIn, nH * nW);
	allocation_tableau(ImgOut, OCTET, nTaille);

   //   for (int i=0; i < nTaille; i++)
   // {
   //  if ( ImgIn[i] < S) ImgOut[i]=0; else ImgOut[i]=255;
   //  }


	for (int i=0; i < nH; i++)
		for (int j=0; j < nW; j++) {

			OCTET* pixel = &ImgIn[i*nW+j];

			if (*pixel < s1) {
				*pixel = 0;
			} else if (*pixel > s1 && *pixel < s2){
				*pixel = 64;
			} 
			else if (*pixel > s2 && *pixel < s3){
				*pixel = 128;
			} 
			else if (*pixel > s3 && *pixel < s4){
				*pixel = 192;
			} else {
				*pixel = 255;
			}
		}

		ecrire_image_pgm(cNomImgEcrite, ImgIn,  nH, nW);
		free(ImgIn);
		return 1;
	}
