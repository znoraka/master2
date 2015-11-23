#include <string>
#include <fstream>
#include <math.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>
#include <stdio.h>
#include <string.h>

#include "image_ppm.h"

#include "lossy.h"
#include "lossless.h"

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

int main(int argc, char *argv[])
{

  char cNomImgLue[250];
  char outString[250] = "out.ppm";
  sscanf (argv[1],"%s",cNomImgLue);

  int width, height;
	
  OCTET *in;
  lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &width, &height);
  allocation_tableau(in, OCTET, width * height * 3);
  lire_image_ppm(cNomImgLue, in, width * height);

  OCTET *ycrcb = toYCbCr(in, width, height);
  auto dico = extractDico(ycrcb, width, height, 12);
  auto out = encodeFromDico(dico, ycrcb, width, height);
  ecrire_image_ppm(outString, toRGB(out, width, height), width, height);
  std::cout << psnr(in, toRGB(out, width, height), width, height) << std::endl;
  
  return 0;
}
