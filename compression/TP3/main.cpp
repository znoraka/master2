#include "../lib_c++/ImageBase.cpp"
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>

class Matrix {
public:
  Matrix(const int width, const int height) {
    this->width = width;
    this->height = height;
    m = new int*[width];
    for (int i = 0; i < width; i++) {
      m[i] = new int[height];
    }
    
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
	m[i][j] = 0;
      }
    }
  }

  int& at(int x, int y) const {
    return m[x][y];
  }

  int getWidth() const {
    return this->width;
  }

  int getHeight() const {
    return this->height;
  }

  Matrix* mul(Matrix *mx) {
    if(this->width != mx->getHeight() || this->height != mx->getWidth()) return 0;

    int n = mx->width;
    int m = mx->height;
    int p = this->height;

    Matrix *result = new Matrix(n, p);
    
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < p; j++) {
	float sum = 0;
	for (int k = 0; k < m; k++) {
	  sum += mx->at(i, k) * this->at(k, j);
	}
	result->at(j, i) = sum;

	std::cout << "i,j = " << i << ", " << j <<  std::endl;
	// std::cout << this->at(i, j) << std::endl;
	// std::cout << m->at(i, j) << std::endl;
	// result->at(i, j) += this->at(i, j) * m->at(i, j);
      }
    }
    std::cout << "mul ok" << std::endl;
    return result;
  }

  void display() {
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
	std::cout << at(i, j) << " ";
      }
      std::cout << std::endl;
    }

  }
  
private:
  int** m;
  int width;
  int height;
};

int main(int argc, char **argv)
{
  std::srand(std::time(0));
  ///////////////////////////////////////// Exemple d'un seuillage d'image
  char cNomImgLue[250], cNomImgEcrite[250];
  int S = 100;
  
  // sscanf (argv[1],"%s",cNomImgLue) ;
  // sscanf (argv[2],"%s",cNomImgEcrite);
  // sscanf (argv[3],"%d",&S);

  Matrix* m = new Matrix(3, 2);
  m->at(0, 0) = 1;
  m->at(1, 0) = 2;
  m->at(2, 0) = 0;
  m->at(0, 1) = 4;
  m->at(1, 1) = 3;
  m->at(2, 1) = -1;

  Matrix* m2 = new Matrix(2, 3);
  m2->at(0, 0) = 5;
  m2->at(1, 0) = 1;
  m2->at(0, 1) = 2;
  m2->at(1, 1) = 3;
  m2->at(0, 2) = 3;
  m2->at(1, 2) = 4;

  // Matrix* m = new Matrix(2, 2);
  // m->at(0, 0) = 1;
  // m->at(1, 0) = 0;
  // m->at(0, 1) = -1;
  // m->at(1, 1) = 3;

  // Matrix* m2 = new Matrix(2, 2);
  // m2->at(0, 0) = 3;
  // m2->at(1, 0) = 1;
  // m2->at(0, 1) = 2;
  // m2->at(1, 1) = 1;

  m->mul(m2)->display();

  // int width, height;
	
  // OCTET *in, *out;
  // lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &width, &height);
  // allocation_tableau(in, OCTET, width * height);
  // lire_image_pgm(cNomImgLue, in, width * height);
  // ecrire_image_pgm(c5, binariseMixte, width, height);  
  return 0;
}
