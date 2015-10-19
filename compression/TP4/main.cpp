#include "../lib_c++/ImageBase.cpp"
#include "huffman.h"
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <stack>

void histo(OCTET* in, int width, int height) {
  int *h = new int[256];
  for (int i = 0; i < 256; i++) {
    h[i] = 0;
  }

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      h[at(in, width, height, i, j, 0)]++;
    }
  }

  for (int i = 0; i < 256; i++) {
    std::cout << i << " " <<  h[i] << std::endl;
  }
}

char toChar(std::stack<bool> *vec) {
  char c = 0;
  for (int i = 0; i < 8; i++) {
    if(vec->top()) c += pow(2, 7 - i);
    vec->pop();
  }
  // vec->erase(vec->begin(), vec->begin() + 8);
  return c;
}

int main(int argc, char **argv)
{
  std::srand(std::time(0));
  ///////////////////////////////////////// Exemple d'un seuillage d'image
  char cNomImgLue[250], cNomImgEcrite[250];
  int S = 100;
  
  sscanf (argv[1],"%s",cNomImgLue) ;
  // sscanf (argv[2],"%s",cNomImgEcrite);
  // sscanf (argv[3],"%d",&S);

  int width, height;
	
  OCTET *in, *out;
  lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &width, &height);
  allocation_tableau(in, OCTET, width * height);
  lire_image_pgm(cNomImgLue, in, width * height);


   int frequencies[UniqueSymbols] = {0};
 
   for (int i = 0; i < width; i++) {
     for (int j = 0; j < height; j++) {
       frequencies[at(in, width, height, i, j, 0)]++;
     }
   }
    INode* root = BuildTree(frequencies);

    HuffCodeMap codes;
    GenerateCodes(root, HuffCode(), codes);
    delete root;

    std::stack<bool> *vec = new std::stack<bool>();
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
	HuffCode c = codes.find(at(in, width, height, i, j, 0))->second;
	std::for_each(c.begin(), c.end(), [=](bool b) {
	    vec->push(b);
	  });
	if(vec->size() > 8) {
	  std::cout << toChar(vec);
	}
      }
    }
  return 0;
}
