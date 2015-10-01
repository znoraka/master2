#include "../lib_c++/ImageBase.cpp"
#include <stdio.h>
#include <iostream>
#include <cstring>

int getThreshold(OCTET* in, int width, int height) {
  int min = 255;
  int max = 0;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      char v = at(in, width, height, i, j, 0);
      if(v > max) max = v;
      if(v < min) min = v;
    }
  }
  return (max - min) * 0.5f;
}

OCTET* binarisation(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height);
  int th = getThreshold(in, width, height);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      
      if(at(in, width, height, i, j, 0) > th)
	at(out, width, height, i, j, 0) = 255;
      else
	at(out, width, height, i, j, 0) = 0;
    }
  }
  return out;
}

int voteResult(OCTET* in, int width, int height, int posX, int posY, int modeSize, int threshold) {
  int balance = 0;
  for (int i = 0; i < modeSize; i++) {
    for (int j = 0; j < modeSize; j++) {
      balance += (at(in, width, height, i + posX, j + posY, 0) > threshold) ? 1 : -1;
    }
  }
  if (balance > 0) return 255;
  return 0;
}

OCTET* binarisationVote(OCTET* in, int width, int height, int modeSize) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height);
  int th = getThreshold(in, width, height);
  for (int i = 0; i < width; i += modeSize) {
    for (int j = 0; j < height; j += modeSize) {
      int vote = voteResult(in, width, height, i, j, modeSize, th);
      for (int x = 0; x < modeSize; x++) {
	for (int y = 0; y < modeSize; y++) {
	  at(out, width, height, i + x, j + y, 0) = vote;
	}
      }
    }
  }
  return out;
}

float windowAvg(OCTET* in, int width, int height, int x, int y, int windowSize) {
  float avg = 0;
  for (int i = 0; i < windowSize; i++) {
    for (int j = 0; j < windowSize; j++) {
      avg += at(in, width, height, i + x, j + y, 0);
    }
  }
  return avg / (windowSize * windowSize);
}

float stdDeviation(OCTET* in, int width, int height, int x, int y, int windowSize) {
  float stdD = 0;
  float avg = windowAvg(in, width, height, x, y, windowSize);

  float sum = 0;
  
  for (int i = 0; i < windowSize; i++) {
    for (int j = 0; j < windowSize; j++) {
      sum += pow(at(in, width, height, i + x, j + y, 0) - avg, 2);
    }
  }

  sum /= pow(windowSize, 2);

  return sqrt(sum);
}

OCTET* niblack(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height);
  int th = getThreshold(in, width, height);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      
      if(at(in, width, height, i, j, 0) > th)
	at(out, width, height, i, j, 0) = 255;
      else
	at(out, width, height, i, j, 0) = 0;
    }
  }
  return out;
}

int main(int argc, char **argv)
{
  ///////////////////////////////////////// Exemple d'un seuillage d'image
  char cNomImgLue[250], cNomImgEcrite[250];
  int S = 100;
  
  sscanf (argv[1],"%s",cNomImgLue) ;
  sscanf (argv[2],"%s",cNomImgEcrite);
  sscanf (argv[3],"%d",&S);

  int width, height;
	
  OCTET *in, *out;
  lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &width, &height);
  allocation_tableau(in, OCTET, width * height);
  lire_image_pgm(cNomImgLue, in, width * height);

  //  allocation_tableau(out, OCTET, width * height);
  //  OCTET* binarise = binarisation(in, width, height);
  OCTET* binarise = binarisationVote(in, width, height, S);
  //  ecrire_image_pgm(cNomImgEcrite, out, width, height);
  ecrire_image_pgm(cNomImgEcrite, binarise, width, height);  
  return 0;
}