#include "../lib_c++/ImageBase.cpp"
#include "huffman.h"
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <stack>
#include <functional>

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
  return c;
}

void huffman(OCTET *in, int width, int height) {
  int frequencies[UniqueSymbols] = {0};
 
  for (int i = 0; i < width * height; i++) {
    frequencies[in[i]]++;
  }

  INode* root = BuildTree(frequencies);

  HuffCodeMap codes;
  GenerateCodes(root, HuffCode(), codes);
  delete root;
  std::stack<bool> *vec = new std::stack<bool>();
  for (int i = 0; i < width * height; i++) {
    HuffCode c = codes.find(in[i])->second;
    std::for_each(c.begin(), c.end(), [=](bool b) {
	vec->push(b);
      });
    if(vec->size() > 8) {
      std::cout << toChar(vec);
    }
  }
}

void codagePredictif(OCTET *in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height);

  std::vector<int> diffs;

  auto topPixelDifference = [=] (int i, int j, int value) {
    return at(in, width, height, i, j - 1, 0) - value;
  };

  auto leftPixelDifference = [=] (int i, int j, int value) {
    return at(in, width, height, i - 1, j, 0) - value;
  };

  auto toBitSet = [] (int value) {
    std::vector<bool> s;
    bool negative = (value < 0);
    value = fabs(value);
    
    if (value >= pow(2, 6)) {
      value = pow(2, 6) - 1;
    }

    // std::cout << value << " = ";

    value /= 2; s.push_back(negative);

    while (value != 0) {
      s.push_back(value % 2);
      value /= 2;
    }

    while (s.size() < 8) s.push_back(0);
    return s;
  };

  diffs.push_back(in[0]);
  //0 si pixel du dessus
  //1 si pixel de gauche
  
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      int value = at(in, width, height, i, j, 0);
      int tpd = 255;
      int lpd = 255;
      
      if (i == 0 && j == 0) continue;

      if (i == 0) {
	//prendre le pixel du dessus
	tpd = topPixelDifference(i, j, value);
      } else if (j == 0) {
	//prendre le pixel de gauche
	lpd = leftPixelDifference(i, j, value);
      } else {
	lpd = leftPixelDifference(i, j, value);
	tpd = topPixelDifference(i, j, value);
      }

      if(fabs(lpd) < fabs(tpd)) {
	diffs.push_back(lpd);
	//TODO noter si c'est left ou top qui est utilisé
	//     c'est le bit de poids fort qui s'en charge
      } else {
	diffs.push_back(tpd);	
      }
      out[diffs.size() - 1] = diffs[diffs.size() - 1] + 128;
      // std::cout << diffs[diffs.size() - 1] << std::endl;
    }
  }
  //codage de la valeur sur 6 bits plus 1 bit de signe
  // toBitSet(-46);
  std::stack<bool> *vec = new std::stack<bool>();
  std::vector<OCTET> *octets = new std::vector<OCTET>();
  std::for_each (diffs.begin(), diffs.end(), [=](int n) {
      std::vector<bool> c = toBitSet(n);
      std::for_each(c.begin(), c.end(), [=](bool b) {
	  vec->push(b);
	});
      if(vec->size() > 8) {
	octets->push_back(toChar(vec));
	// std::cout << toChar(vec);
      }
    });
  huffman(&(*octets)[0], octets->size(), 1);
  char a[] = "out.pgm";
  ecrire_image_pgm(a, out, width, height);
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
  lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &width, &height);
  allocation_tableau(in, OCTET, width * height * 3);
  lire_image_ppm(cNomImgLue, in, width * height);

  // codagePredictif(in, width, height);
  huffman(in, width, height);
  return 0;
}
