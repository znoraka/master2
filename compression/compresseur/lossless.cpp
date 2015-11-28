#include "lossless.h"

void writeRLEToFile(OCTET* encodedImage, int width, int height, std::string path) {
  //3 channels écrits séparéments, les uns après les autres
  //R -> G -> B

  std::ofstream file(path);

  
  for (int j = 0; j < 3; j++) {
    int length = 1;
    OCTET last = encodedImage[0 + j];
    for (int i = 1 + j; i < width * height; i++) {
      if(encodedImage[i * 3 + j] != last) {
	file << (OCTET) length << last;
	length = 1;
	last = encodedImage[i * 3 + j];
      } else {
	length++;
      }
    }
  }

  file.close();
}
