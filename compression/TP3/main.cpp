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
    m = new int*[height];
    for (int i = 0; i < height; i++) {
      m[i] = new int[width];
    }

    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
	m[i][j] = 0;
      }
    }
  }

  static Matrix *hamming() {
    static Matrix* hammingMatrix = new Matrix(4, 7);
    static bool b = false;
    
    if(!b) {
      static int array0[4] = {1, 1, 0, 1}; hammingMatrix->m[0] = array0;
      static int array1[4] = {1, 0, 1, 1}; hammingMatrix->m[1] = array1;
      static int array2[4] = {1, 0, 0, 0}; hammingMatrix->m[2] = array2;
      static int array3[4] = {0, 1, 1, 1}; hammingMatrix->m[3] = array3;
      static int array4[4] = {0, 1, 0, 0}; hammingMatrix->m[4] = array4;
      static int array5[4] = {0, 0, 1, 0}; hammingMatrix->m[5] = array5;
      static int array6[4] = {0, 0, 0, 1}; hammingMatrix->m[6] = array6;
      b = true;
    }    
    return hammingMatrix;
  }

  Matrix(int **array, const int width, const int height) {
    this->width = width;
    this->height = height;
    this->m = array;
  }

  Matrix *rotate() {
    int **newm = new int*[width];
    for (int i = 0; i < width; i++) {
      newm[i] = new int[height];
    }
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
	newm[i][j] = m[j][i];
      }
    }

    int temp = this->height;
    this->height = this->width;
    this->width = temp;
    // delete newm;
    m = newm;
    return this;
  }
    

  int& at(int x, int y) const {
    return m[y][x];
  }

  int getWidth() const {
    return this->width;
  }

  int getHeight() const {
    return this->height;
  }

  Matrix* mul(Matrix *mx) {
    if(this->width != mx->getHeight()) return 0;

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
	result->at(i, j) = sum;
      }
    }
    return result;
  }

  Matrix *toBinary() {
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
	at(i, j) = at(i, j) % 2;
      }
    }
    return this;
  }

  void display() {
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
	std::cout << at(j, i) << " ";
      }
      std::cout << std::endl;
    }
  }

private:
  int** m;
  int width;
  int height;
};

Matrix *encodeMessage(int message[], int size) {
  Matrix m(&message, size, 1);
  m.rotate();
  Matrix *res = Matrix::hamming()->mul(&m);
  if(res == 0) std::cout << "error" << std::endl;
  res->toBinary();
  return res;
}

int main(int argc, char **argv)
{
  std::srand(std::time(0));
  char cNomImgLue[250], cNomImgEcrite[250];
  int S = 100;

  // Matrix* m = new Matrix(3, 2);
  // m->at(0, 0) = 1;
  // m->at(1, 0) = 2;
  // m->at(2, 0) = 0;
  // m->at(0, 1) = 4;
  // m->at(1, 1) = 3;
  // m->at(2, 1) = -1;

  // Matrix* m2 = new Matrix(2, 3);
  // m2->at(0, 0) = 5;
  // m2->at(1, 0) = 1;
  // m2->at(0, 1) = 2;
  // m2->at(1, 1) = 3;
  // m2->at(0, 2) = 3;
  // m2->at(1, 2) = 4;

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

  // m->mul(m2)->display();  m2->mul(m)->display();

  // Matrix::hamming()->display();
  int message[4] = {1, 0, 1, 1};
  encodeMessage(message, 4)->display();
  return 0;
}
