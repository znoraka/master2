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
  mse /= (width * height * 3.0);
  return 10 * log10(pow(255, 2) /  mse);
}

int main(int argc, char *argv[])
{
  char cNomImgLue[250];

  sscanf (argv[1],"%s",cNomImgLue);

  std::string filename = cNomImgLue;
  std::string ext = filename.substr(filename.length() - 4, 4);

  int dicoSize = 5;

  int width, height;
	
  OCTET *in;
  
  if(ext.compare(".ppm") == 0) {
    if(argc > 2) {
      sscanf (argv[2], "%d", &dicoSize);
    }

    std::cout << "this is a ppm file!" << std::endl;

    lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &width, &height);
    allocation_tableau(in, OCTET, width * height * 3);
    lire_image_ppm(cNomImgLue, in, width * height);


  } else if(ext.compare(".pgm") == 0) {
    if(argc > 2) {
      sscanf (argv[2], "%d", &dicoSize);
    }
 
    std::cout << "this is a pgm file!" << std::endl;

    lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &width, &height);
    allocation_tableau(in, OCTET, width * height * 3);
    std::vector<OCTET> temp; temp.resize(width * height);
    lire_image_pgm(cNomImgLue, &temp[0], width * height);
    
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
	at(in, width, height, i, j, RED) = at(&temp[0], width, height, i, j, 0);
	at(in, width, height, i, j, GREEN) = at(&temp[0], width, height, i, j, 0);
	at(in, width, height, i, j, BLUE) = at(&temp[0], width, height, i, j, 0);
      }
    }
  } else if(ext.compare(".tmp") == 0) {

    std::ifstream stream("out.tmp", std::ifstream::binary);
    auto outImg = decodeFromDico(stream, width, height);

    char name[250];

    if(argc > 2) {
      sscanf (argv[2], "%s", name);
    }
 
    ecrire_image_ppm(name, toRGB(&outImg[0], width, height), width, height);
    return 0;

  } else {

    std::cout << "I can't process this file" << std::endl;
    return -1;

  }

  if(dicoSize > 8) dicoSize = 8; if(dicoSize < 1) dicoSize = 1;

  OCTET *ycrcb = toYCbCr(in, width, height);

  auto out = resizeImageChannel(ycrcb, width, height, 32, 32, Cr);
  out = resizeImageChannel(&out[0], width, height, 32, 32, Cb);
  // out = resizeImageChannel(&out[0], width, height, 8, 8, Yc);
  std::vector<std::vector<OCTET> > dico;
  if(dicoSize > 5) {
    dico = extractDicoMediaCut(&out[0], width, height, dicoSize);
  } else {
    dico = extractDicoKmeans(&out[0], width, height, dicoSize);
  }
  auto encoded = encodeFromDico(dico, ycrcb, width, height);
  // char name[] = "../compiled/temp.ppm";
  // ecrire_image_ppm(name, toRGB(encoded, width, height), width, height);

  exportPaletteToPpm(dico, "../compiled/palette.ppm");

  std::cout << "psnr = " << psnr(ycrcb, encoded, width, height) << std::endl;
  std::ofstream file("out.tmp");
  // writeDicoToStream(dico, &out[0], width, height, file, dicoSize);
  writeDicoToStream(dico, ycrcb, width, height, file, dicoSize);
  file.close();

  return 0;
}
