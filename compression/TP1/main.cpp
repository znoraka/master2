#include "../lib_c++/ImageBase.cpp"
#include <stdio.h>
#include <iostream>
#include <cstring>


#define RED 1
#define GREEN 2
#define BLUE 3

OCTET* resizeImageChannel(OCTET* in, int width, int height, int outWidth, int outHeight, int color) {
  std::cout << width/outWidth << std::endl;
  OCTET *out;
  OCTET *out1;
  allocation_tableau(out, OCTET, outWidth * outHeight);
  allocation_tableau(out1, OCTET, width * height);

  int diff = width/outWidth;
    
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      at(out, outWidth, outHeight, i / diff, j / diff, 0) = at(in, width, height, i, j, color);
    }
  }

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      //std::cout << in[i / diff * width + j / diff] << std::endl;
      //      out[i * width + j] = in[i / diff * width + j / diff];
      //      std::cout << j * diff << std::endl;
      out1[i * width + j] = at(out, outWidth, outHeight, i / diff, j / diff, 0);
    }
  }

  return out1;
}

OCTET* resizeColor(OCTET* in, int width, int height, int outWidth, int outHeight) {
  OCTET *out;
  allocation_tableau(out, OCTET, outWidth * outHeight);
 
  int diff = outWidth/width;
    
  for (int i = 0; i < outWidth; i++) {
    for (int j = 0; j < outHeight; j++) {
      //std::cout << in[i / diff * width + j / diff] << std::endl;
      //      out[i * outWidth + j] = in[i / diff * width + j / diff];
      //      std::cout << j * diff << std::endl;
      out[i * outWidth + j] = at(in, width, height, i / diff, j / diff, 0);
    }
  }
  std::cout << "resize ok" << std::endl;
  return out;
}


void zizicaca(OCTET* in, OCTET* out, int width, int height) {

  int diff = 2;
  
  OCTET* red = resizeImageChannel(in, width, height, width, height, RED);
  OCTET* green = resizeImageChannel(in, width, height, width / diff, height / diff, GREEN);
  OCTET* blue = resizeImageChannel(in, width, height, width / diff, height / diff, BLUE);
  
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      at(out, width, height, i, j, RED) = red[i * width + j];
      at(out, width, height, i, j, GREEN) = green[i * width + j];
      at(out, width, height, i, j, BLUE) = blue[i * width + j];
    }
  }
}

int main(int argc, char **argv)
{
  ///////////////////////////////////////// Exemple d'un seuillage d'image
  char cNomImgLue[250], cNomImgEcrite[250];
  int S = 100;
  
  if (argc != 3) 
    {
      printf("Usage: ImageIn.pgm ImageOut.pgm \n"); 
      return 1;
    }
  sscanf (argv[1],"%s",cNomImgLue) ;
  sscanf (argv[2],"%s",cNomImgEcrite);

  int width, height;
	
  OCTET *in, *out;
  lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &width, &height);
  allocation_tableau(in, OCTET, width * height * 3);
  lire_image_ppm(cNomImgLue, in, width * height);

  allocation_tableau(out, OCTET, width * height * 3);

  zizicaca(in, out, width, height);
  std::cout << "zizi caca ok" << std::endl;
  ecrire_image_ppm(cNomImgEcrite, out, width, height);  
  return 0;
}
