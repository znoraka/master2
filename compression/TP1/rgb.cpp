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

OCTET* toYCbCr(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height * 3);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      
      at(out, width, height, i, j, RED) =
	fmin(500, 
	     at(in, width, height, i, j, RED) * 0.299f +
	     at(in, width, height, i, j, GREEN) * 0.587f +
	     at(in, width, height, i, j, BLUE) * 0.114f);

      at(out, width, height, i, j, GREEN) =
	fmin(500,
	     128 -
	     at(in, width, height, i, j, RED) * 0.168736f -
	     at(in, width, height, i, j, GREEN) * 0.331264f +
	     at(in, width, height, i, j, BLUE) * 0.5f);

      at(out, width, height, i, j, BLUE) =
	fmin(500, 
	     128 +
	     at(in, width, height, i, j, RED) * 0.5f -
	     at(in, width, height, i, j, GREEN) * 0.418688f -
	     at(in, width, height, i, j, BLUE) * 0.081312f);
    }
  }
  return out;
}

OCTET* toRGB(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height * 3);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      
      at(out, width, height, i, j, RED) = 
	fmin(500,
	     at(in, width, height, i, j, RED) +
	     1.402f * (at(in, width, height, i, j, BLUE) - 128));

      at(out, width, height, i, j, GREEN) =
	fmin(500, 
	     at(in, width, height, i, j, RED) -
	     0.34414f * (at(in, width, height, i, j, GREEN) - 128) -
	     0.71414f * (at(in, width, height, i, j, BLUE) - 128));

      at(out, width, height, i, j, BLUE) =
	fmin(500,
	     at(in, width, height, i, j, RED) +
	     1.772f * (at(in, width, height, i, j, GREEN) - 128));
    }
  }
  return out;
}


void zizicaca(OCTET* in, OCTET* out, int width, int height) {

  int diff = 2;
  
  OCTET* red = resizeImageChannel(in, width, height, width / diff , height / diff,  RED);
  OCTET* green = resizeImageChannel(in, width, height, width,  height,  GREEN);
  OCTET* blue = resizeImageChannel(in, width, height, width / diff, height / diff, BLUE);
  
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      at(out, width, height, i, j, RED) = red[i * width + j];
      at(out, width, height, i, j, GREEN) = green[i * width + j];
      at(out, width, height, i, j, BLUE) = blue[i * width + j];
    }
  }
}

float psnr(OCTET* in, OCTET* compressed, int width, int height) {
  double mse = 0;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      mse += pow(at(in, width, height, i, j, RED) - at(compressed, width, height, i, j, RED), 2);
      mse += pow(at(in, width, height, i, j, GREEN) - at(compressed, width, height, i, j, GREEN), 2);
      mse += pow(at(in, width, height, i, j, BLUE) - at(compressed, width, height, i, j, BLUE), 2);
    }
  }
  mse /= (width * height * 3);
  return 10 * log10(pow(255, 2) /  mse);
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

  OCTET* o1 = toYCbCr(in, width, height);
  out = toRGB(o1, width, height);

  zizicaca(in, out, width, height);
  //zizicaca(o1, out, width, height);
  //  out = toRGB(out, width, height);
  std::cout << "psnr = " << psnr(in, out, width, height) << std::endl;
  ecrire_image_ppm(cNomImgEcrite, out, width, height);  
  return 0;
}
