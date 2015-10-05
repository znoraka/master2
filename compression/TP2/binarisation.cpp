#include "../lib_c++/ImageBase.cpp"
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>

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
  std::cout << "threshold = " << th << std::endl;
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
  for (int i = -windowSize * 0.5f; i < windowSize * 0.5f; i++) {
    for (int j = -windowSize * 0.5f; j < windowSize * 0.5f; j++) {
      int a = i + x;
      int b = j + y;
      if(a < 0) a = 0; if(a >= width) a = width - 1;
      if(b < 0) b = 0; if(b >= height) b = height - 1;
      avg += at(in, width, height, a, b, 0);
    }
  }
  return avg / (windowSize * windowSize);
}

float stdDeviation(OCTET* in, int width, int height, int x, int y, int windowSize, float avg) {
  float sum = 0;

  for (int i = -windowSize * 0.5f; i < windowSize * 0.5f; i++) {
    for (int j = -windowSize * 0.5f; j < windowSize * 0.5f; j++) {
      int a = i + x;
      int b = j + y;
      if(a < 0) a = 0; if(a >= width) a = width - 1;
      if(b < 0) b = 0; if(b >= height) b = height - 1;
      sum += pow(at(in, width, height, a, b, 0) - avg, 2);
    }
  }

    sum /= pow(windowSize, 2);
  //std::cout << "ecart type = " << sqrt(sum) << std::endl;

  return sqrt(sum);
}

float getNiblackThreshold(OCTET* in, int width, int height, int x, int y, int windowSize) {
   float avg = windowAvg(in, width, height, x, y, windowSize);
   float stdD = stdDeviation(in, width, height, x, y, windowSize, avg);
   // std::cout << stdD << std::endl;
   return avg - 0.3f * stdD;
   // return avg;
}

OCTET* niblack(OCTET* in, int width, int height, int windowSize) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height);
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      int th = getNiblackThreshold(in, width, height, i, j, windowSize);
      if(at(in, width, height, i, j, 0) > th)
      	at(out, width, height, i, j, 0) = 255;
      else
      	at(out, width, height, i, j, 0) = 0;
    }
  }
  return out;
}

float getMixedThreshold(OCTET* in, int width, int height) {
  float windowSize = fmin(width * 1.0f, 100000);
  float maxTh = 0;
  float minTh = 255;

  for (int i = 0; i < 9; i++) {
    int x = std::rand() % width;
    int y = std::rand() % height;

    std::cout << x << std::endl;
    std::cout << y << std::endl;
    
    float avg = windowAvg(in, width, height, x, y, windowSize);

    std::cout << avg << std::endl;

    maxTh = fmax(avg, maxTh);
    minTh = fmin(avg, minTh);
  }
  return (maxTh + minTh) * 0.5f;
}

OCTET* mixte(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height);
  int th = getMixedThreshold(in, width, height);
  std::cout << "mixed threshold = " << th << std::endl;
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

int numberOfBlackPixels(OCTET* in, int width, int height) {
  int n = 0;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      if(at(in, width, height, i, j, 0) == 0) n++;
    }
  }
  return n;
}

int main(int argc, char **argv)
{
  std::srand(std::time(0));
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
  OCTET* binarise = binarisation(in, width, height);
  OCTET* binariseVote = binarisationVote(in, width, height, S);
  OCTET* binariseNiblack = niblack(in, width, height, S * 2.5);
  OCTET* squareniblack = binarisationVote(binariseNiblack, width, height, S);
  OCTET* binariseMixte = mixte(in, width, height);
  
  std::cout << "blacks binarise = " << numberOfBlackPixels(binarise, width, height) << std::endl;
  std::cout << "whites binarise = " << width * height - numberOfBlackPixels(binarise, width, height) << std::endl;
  std::cout << "black = " << 100 * numberOfBlackPixels(binarise, width, height) / (width * height) << "%" << std::endl << "\n";

  std::cout << "blacks vote = " << numberOfBlackPixels(binariseVote, width, height) << std::endl;
  std::cout << "whites vote = " << width * height - numberOfBlackPixels(binariseVote, width, height) << std::endl;
  std::cout << "black = " << 100 * numberOfBlackPixels(binariseVote, width, height) / (width * height) << "%" << std::endl << "\n";

  std::cout << "blacks niblack = " << numberOfBlackPixels(binariseNiblack, width, height) << std::endl;
  std::cout << "whites niblack = " << width * height - numberOfBlackPixels(binariseNiblack, width, height) << std::endl;
  std::cout << "black = " << 100 * numberOfBlackPixels(binariseNiblack, width, height) / (width * height) << "%" << std::endl << "\n";

  std::cout << "blacks squareNiblack = " << numberOfBlackPixels(squareniblack, width, height) << std::endl;
  std::cout << "whites squareNiblack = " << width * height - numberOfBlackPixels(squareniblack, width, height) << std::endl;
  std::cout << "black = " << 100 * numberOfBlackPixels(squareniblack, width, height) / (width * height) << "%" << std::endl << "\n";

  std::cout << "blacks mixte = " << numberOfBlackPixels(binariseMixte, width, height) << std::endl;
  std::cout << "whites mixte = " << width * height - numberOfBlackPixels(binariseMixte, width, height) << std::endl;
  std::cout << "black = " << 100 * numberOfBlackPixels(binariseMixte, width, height) / (width * height) << "%" << std::endl << "\n";

  char c1 [100] = "../TP2/niblack.pgm";
  char c2 [100] = "../TP2/vote.pgm";
  char c3 [100] = "../TP2/binarise.pgm";
  char c4 [100] = "../TP2/squareniblack.pgm";
  char c5 [100] = "../TP2/binarisemixte.pgm";
  ecrire_image_pgm(c1, binariseNiblack, width, height);
  ecrire_image_pgm(c2, binariseVote, width, height);
  ecrire_image_pgm(c3, binarise, width, height);
  ecrire_image_pgm(c4, squareniblack, width, height);
  ecrire_image_pgm(c5, binariseMixte, width, height);  
  return 0;
}
