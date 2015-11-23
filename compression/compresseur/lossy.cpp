#include "lossy.h"

std::vector<std::vector<OCTET>> extractDico(OCTET *in, int width, int height, int dicoSize) {
  //4 bits for channel Y = 2^4 = 16 values
  //2 bits for Cr and Cb = 4 values;
  std::vector<std::vector<int>> histos;
  
  auto clamp = [=](int channel, int values, std::vector<int> histo) {
    std::vector<OCTET> vec;
    int intervalSize = (width * height) / values;
    int intervalStart = 0;
    long sum = 0;
    
    for (int i = 0; i < histo.size(); i++) {
      sum += histo[i];
      if(sum > intervalSize) {
	vec.push_back((i + intervalStart) * 0.5);
	intervalStart = i;
	sum = 0;
      }
    }
    vec.push_back((histo.size() + intervalStart) * 0.5);
    return vec;
  };

  for (int i = 0; i < 3; i++) {
    histos.push_back(std::vector<int>());
  }
  
  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 3; j++) {
      histos[j].push_back(0);
    }
  }

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      histos[0][at(in, width, height, i, j, RED)]++;
      histos[1][at(in, width, height, i, j, GREEN)]++;
      histos[2][at(in, width, height, i, j, BLUE)]++;
    }
  }
  std::vector<std::vector<OCTET>> dico;
  dico.push_back(clamp(Y, pow(2, dicoSize * 0.5), histos[0]));
  dico.push_back(clamp(Cr, pow(2, dicoSize * 0.25), histos[1]));
  dico.push_back(clamp(Cb, pow(2, dicoSize * 0.25), histos[2]));

  return dico;

  // for (int i = 0; i < 256; i++) {
  //     std::cout << i << " ";
  //     std::cout << histos[0][i] << " ";
  //     std::cout << dico[1][i] << " ";
  //     std::cout << dico[2][i] << " ";
  //     std::cout << std::endl;
  // }
}

OCTET* toYCbCr(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height * 3);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {

      double red = at(in, width, height, i, j, RED);
      double green = at(in, width, height, i, j, GREEN);
      double blue = at(in, width, height, i, j, BLUE);

      double y = 0.299 * red + 0.587 * green + 0.114 * blue;
      double u = 128 + 0.492 * (blue - y);
      double v = 128 + 0.877 * (red - y);

      // if(y > 255 || y < 0) std::cout << "y = " << y << std::endl;
      // if(u > 255 || u < 0) std::cout << "u = " << u << std::endl;
      // if(v > 255 || v < 0) std::cout << "v = " << v << std::endl;

      at(out, width, height, i, j, Y) = y;
      at(out, width, height, i, j, Cr) = u;
      at(out, width, height, i, j, Cb) = v;
    }
  }
  return out;
}


OCTET* toRGB(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height * 3);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {

      double y = at(in, width, height, i, j, Y);
      double u = at(in, width, height, i, j, Cr) - 128;
      double v = at(in, width, height, i, j, Cb) - 128;

      double red = y + 1.140 * v;
      double green = y - 0.395 * u - 0.581 * v;
      double blue = y + 2.032 * u;

      // if(red > 255 || red < 0) std::cout << "red = " << red << std::endl;
      // if(green > 255 || green < 0) std::cout << "green = " << green << std::endl;
      // if(blue > 255 || blue < 0) std::cout << "blue = " << blue << std::endl;

      at(out, width, height, i, j, RED) = fmax(0, fmin(255, red));
      at(out, width, height, i, j, GREEN) = fmax(0, fmin(255, green));
      at(out, width, height, i, j, BLUE) = fmax(0, fmin(255, blue));

    }
  }
  return out;
}

OCTET *encodeFromDico(std::vector<std::vector<OCTET>> dico, OCTET *in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height * 3);

  auto getValue = [=](std::vector<OCTET> vec, OCTET o) {
    int best = 255;
    for(auto i : vec) {
      if(abs(i - o) < abs(best - o)) {
	best = i;
      }
    }
    return best;
  };

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      at(out, width, height, i, j, Y) = getValue(dico[0], at(in, width, height, i, j, Y));
      at(out, width, height, i, j, Cr) = getValue(dico[1], at(in, width, height, i, j, Cr));
      at(out, width, height, i, j, Cb) = getValue(dico[2], at(in, width, height, i, j, Cb));
    }
  }
  return out;
}
