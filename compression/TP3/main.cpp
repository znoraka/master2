#include "../lib_c++/ImageBase.cpp"
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

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

std::vector<int> encodeMessage(int message[], int size) {
  std::cout << "matrice génératrice : " << std::endl;
  Matrix::hamming()->display();
  Matrix m(&message, size, 1);
  m.rotate();
  std::cout << "message : " << std::endl;
  m.display();
  Matrix *res = Matrix::hamming()->mul(&m);
  if(res == 0) std::cout << "error" << std::endl;
  res->toBinary();
  std::vector<int> out;
  for (int i = 0; i < res->getHeight(); i++) {
    out.push_back(res->at(0, i));
  }
  return out;
}

OCTET *newModule(int size) {
  OCTET* o = new OCTET[size * size];
  for (int i = 0; i < size * size; i++) {
    o[i] = 0;
  }

  int n = size * size * 0.25;
  while(n >= 0) {
    int r = std::rand() % (size * size);
    if(o[r] == 0) {
      n--;
      o[r] = 255;
    }
  }
  return o;
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

float getNiblackThreshold(OCTET* in, int width, int height, int x, int y, int windowSize) {
  float avg = windowAvg(in, width, height, x, y, windowSize);
  float stdD = stdDeviation(in, width, height, x, y, windowSize, avg);
  // std::cout << stdD << std::endl;
  return avg - 0.3f * stdD;
  // return avg;
}

OCTET* niblack(OCTET* in, int width, int height, int windowSize, int modeSize) {
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
  return binarisationVote(out, width, height, modeSize);
}

OCTET* binarisation(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height);
  int th = getThreshold(in, width, height);
  printf("threshold = %d", th);
  /* std::cout << "threshold = " << th << std::endl; */
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

OCTET* changeModules(OCTET* in, int width, int height, int modeSize, std::vector<int> message) {
  OCTET *out;
  int n = 0;
  allocation_tableau(out, OCTET, width * height);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      at(out, width, height, i, j, 0) = at(in, width, height, i, j, 0);
    }
  }
  for (int i = 0; i < width; i += modeSize) {
    for (int j = 0; j < height; j += modeSize) {
      if(at(in, width, height, i + 2, j, 0) == 0) {
	if(n < message.size() && message[n++] == 1) {
	  OCTET* message = newModule(modeSize);
	  int cpt = 0;
	  for (int x = 0; x < modeSize; x++) {
	    for (int y = 0; y < modeSize; y++) {
	      at(out, width, height, i + x, j + y, 0) = message[cpt++];
	    }
	  }
	}
      }
    }
  }
  return out;
}

std::vector<int> generateRandomMessage(int length) {
  std::vector<int> vec;
  for (int i = 0; i < length; i++) {
    vec.push_back(std::rand() % 2);
  }
  return vec;
}

OCTET *toGrayScale(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      at(out, width, height, i, j, 0) =
	at(in, width, height, i, j, RED) * 0.29 +
	at(in, width, height, i, j, GREEN) * 0.59 +
	at(in, width, height, i, j, BLUE) * 0.12;
    }
  }
  return out;
}

OCTET* assemblage(OCTET* q, int qwidth, int qheight,
		  OCTET* c, int cwidth, int cheight) {
  OCTET *out;
  allocation_tableau(out, OCTET, cwidth * cheight);

  OCTET tu = 80;
  OCTET tl = 100;

  for (int i = 0; i < cwidth; i++) {
    for (int j = 0; j < cheight; j++) {
      at(out, cwidth, cheight, i, j, 0) = at(c, cwidth, cheight, i, j, 0);
    }
  }

  for (int i = 0; i < qwidth; i++) {
    for (int j = 0; j < qheight; j++) {
      if(at(q, qwidth, qheight, i, j, 0) <= 20) {
	at(out, cwidth, cheight, i, j, 0) = fmax(at(c, cwidth, cheight, i, j, 0), tu);
      } else {
	at(out, cwidth, cheight, i, j, 0) = fmin(at(c, cwidth, cheight, i, j, 0), tl);
      }
    }
  }
  return out;
}

int main(int argc, char **argv)
{
  std::srand(std::time(0));
  int message[4] = {1, 0, 1, 0};
  std::vector<int> res = encodeMessage(message, 4);
  // std::vector<int> res = generateRandomMessage(32);
  std::cout << "résultat : " << std::endl;
  std::for_each(res.begin(), res.end(), [](int a) {
      std::cout << a << " ";
    });
  std::cout  << std::endl;

  char cNomImgLue[250], cNomImgEcrite[250], cNomImgLue1[250];
  int S = 100;
  
  sscanf (argv[1],"%s",cNomImgLue) ;
  sscanf (argv[2],"%s",cNomImgLue1) ;
  sscanf (argv[3],"%s",cNomImgEcrite);
  sscanf (argv[4],"%d",&S);

  int width, height;
  int bcwidth, bcheight;
  OCTET *in, *out, *barcode;
  lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &width, &height);
  allocation_tableau(in, OCTET, width * height * 3);
  lire_image_ppm(cNomImgLue, in, width * height);

  lire_nb_lignes_colonnes_image_pgm(cNomImgLue1, &bcwidth, &bcheight);
  allocation_tableau(barcode, OCTET, bcwidth * bcheight);
  lire_image_pgm(cNomImgLue1, barcode, bcwidth * bcheight);
  // OCTET* assemble = assemblage(barcode, bcwidth, bcheight, toGrayScale(in, width, height), width, height);
  ecrire_image_pgm(cNomImgEcrite, assemblage(barcode, bcwidth, bcheight, toGrayScale(in, width, height), width, height), width, height);
  
  // int width, height;
	
  // OCTET *in, *out;
  // lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &width, &height);
  // allocation_tableau(in, OCTET, width * height);
  // lire_image_pgm(cNomImgLue, in, width * height);

  // char c4 [100] = "../TP3/squareniblack.pgm";
  // char c5 [100] = "../TP3/inserted.pgm";
  
  // OCTET* niblackOut = niblack(in, width, height, fmin(width / 8.0f, 20), S);
  // OCTET* inserted = changeModules(niblackOut, width, height, S, res);
  // ecrire_image_pgm(c4, niblackOut, width, height);
  // ecrire_image_pgm(c5, inserted, width, height);
  return 0;
}
