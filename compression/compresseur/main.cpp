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

  std::string filename = cNomImgLue;
  std::string ext = filename.substr(filename.length() - 4, 4);

  int dicoSize = 5;
  int width, height;
	
  OCTET *in;


  if(ext.compare(".ppm") == 0) {

    std::cout << "this is a ppm file!" << std::endl;
    dicoSize = 5;

    lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &width, &height);
    allocation_tableau(in, OCTET, width * height * 3);
    lire_image_ppm(cNomImgLue, in, width * height);


  } else if(ext.compare(".pgm") == 0) {

    std::cout << "this is a pgm file!" << std::endl;
    dicoSize = 4;

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


  } else {

    std::cout << "I can't process this file" << std::endl;
    return -1;

  }

  OCTET *ycrcb = toYCbCr(in, width, height);
  // // auto dico = extractDico(ycrcb, width, height, 12);

  // auto out = encodeFromDico(dico, ycrcb, width, height);
  auto out = resizeImageChannel(ycrcb, width, height, 64, 64, Cr);
  out = resizeImageChannel(&out[0], width, height, 64, 64, Cb);
  auto dico = extractDicoKmeans(&out[0], width, height, dicoSize);
  auto encoded = encodeFromDico(dico, ycrcb, width, height);
  // ecrire_image_ppm(outString, &out[0], width, height);
  std::cout << psnr(ycrcb, encoded, width, height) << std::endl;
  std::ofstream file("out.dat");
  writeDicoToStream(dico, ycrcb, width, height, file, dicoSize);
  file.close();

  std::ifstream stream("out.dat", std::ifstream::binary);
  auto outImg = decodeFromDico(stream);

  char name[] = "decoded.ppm";
  ecrire_image_ppm(name, toRGB(&outImg[0], width, height), width, height);
  
  // writeRLEToFile(out, width, height, "out.dat");
  return 0;
}
