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

std::vector<std::vector<OCTET>> extractDicoKmeans(OCTET *in, int width, int height, int dicoSize) {
  int K = pow(2, dicoSize);
  point v = colorToPoint(in, width, height);
  point c = lloyd(v, width * height, K);
  std::vector<std::vector<long>> sums;
  std::vector<int> counts;
  std::vector<std::vector<OCTET>> dico;

  for (int i = 0; i < 3; i++) {
    sums.push_back(std::vector<long>());
    dico.push_back(std::vector<OCTET>());
  }

  for (int i = 0; i < K; i++) {
    counts.push_back(0);
    for (int j = 0; j < 3; j++) {
      sums[j].push_back(0);
      dico[j].push_back(0);
      
    }
  }

  for (int i = 0; i < width * height; i++) {
    point p = v + i;
    counts[p->group]++;
    sums[0][p->group] += p->x;
    sums[1][p->group] += p->y;
    sums[2][p->group] += p->z;
  }

  for (int i = 0; i < K; i++) {
    dico[0][i] = sums[0][i] / counts[i];
    dico[1][i] = sums[1][i] / counts[i];
    dico[2][i] = sums[2][i] / counts[i];
  }
  
  return dico;
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

  auto dist2 = [=](int a, int b) {
    double x = in[a * 3 + 0] - dico[0][b];
    double y = in[a * 3 + 1] - dico[1][b];
    double z = in[a * 3 + 2] - dico[2][b];
    return x*x + y*y + z*z;
  };

  auto getValue = [=](int index) {
    int best = 0;

    for (int i = 0; i < dico[0].size(); i++) {
      if(dist2(index, i) < dist2(index, best)) {
	best = i;
      }
    }

    return best;
  };

  for (int i = 0; i < width * height; i++) {
    auto value = getValue(i);
    out[i * 3 + 0] = dico[0][value];
    out[i * 3 + 1] = dico[1][value];
    out[i * 3 + 2] = dico[2][value];
  }

  return out;
}

void writeDicoToFile(std::vector<std::vector<OCTET>> dico, OCTET* in, int width, int height, std::string path, int dicoValueSize) {
  std::ofstream file(path);
  
  auto toBitSet = [=] (int value) {
    std::vector<bool> s;
    
    while (value != 0) {
      s.push_back(value % 2);
      value /= 2;
    }

    while (s.size() < dicoValueSize) s.push_back(0);
    return s;
  };

    auto dist2 = [=](int a, int b) {
    double x = in[a * 3 + 0] - dico[0][b];
    double y = in[a * 3 + 1] - dico[1][b];
    double z = in[a * 3 + 2] - dico[2][b];
    return x*x + y*y + z*z;
  };

  auto getValue = [=](int index) {
    int best = 0;

    for (int i = 0; i < dico[0].size(); i++) {
      if(dist2(index, i) < dist2(index, best)) {
	best = i;
      }
    }

    return best;
  };

  auto toChar = [=](std::stack<bool> *vec) {
      char c = 0;
      for (int i = 0; i < 8; i++) {
	if(vec->top()) c += pow(2, 7 - i);
	vec->pop();
      }
      return c;
    };


  for(auto vec : dico) {
    for(auto i : vec) {
      file << i;
    }
  }
    
  std::stack<bool> *s = new std::stack<bool>();
  for (int i = 0; i < width * height; i++) {
    auto value = getValue(i);
    auto vec = toBitSet(value);

    assert(vec.size() == dicoValueSize);

    for(auto v : vec) {
      s->push(v);
    }
    if(s->size() > 8) {
      file << toChar(s);
    }
  }
  file.close();
}
