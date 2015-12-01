#include "lossy.h"

std::vector<std::vector<OCTET> > extractDico(OCTET *in, int width, int height, int dicoSize) {
  //4 bits for channel Y = 2^4 = 16 values
  //2 bits for Cr and Cb = 4 values;
  std::vector<std::vector<int> > histos;
  
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
  std::vector<std::vector<OCTET> > dico;
  dico.push_back(clamp(Yc, pow(2, dicoSize * 0.5), histos[0]));
  dico.push_back(clamp(Cr, pow(2, dicoSize * 0.25), histos[1]));
  dico.push_back(clamp(Cb, pow(2, dicoSize * 0.25), histos[2]));

  return dico;
}

std::vector<std::vector<OCTET> > extractDicoKmeans(OCTET *in, int width, int height, int dicoSize) {
  int K = pow(2, dicoSize);
  point v = colorToPoint(in, width, height);
  point c = lloyd(v, width * height, K);

  std::vector<std::vector<OCTET> > dico;

  for (int i = 0; i < 3; i++) {
    dico.push_back(std::vector<OCTET>());
  }

  for (int i = 0; i < K; i++) {
    for (int j = 0; j < 3; j++) {
      dico[j].push_back(0);
    }
  }

  for (int i = 0; i < K; i++) {
    point p = c + i;
    dico[0][i] = p->x;
    dico[1][i] = p->y;
    dico[2][i] = p->z;
  }
  
  return dico;
}

std::vector<std::vector<OCTET> > extractDicoMediaCut(OCTET *in, int width, int height, int dicoSize) {
  int K = pow(2, dicoSize);

  std::vector<std::vector<OCTET> > dico;

  for (int i = 0; i < 3; i++) {
    dico.push_back(std::vector<OCTET>());
  }

  for (int i = 0; i < K; i++) {
    for (int j = 0; j < 3; j++) {
      dico[j].push_back(0);
    }
  }

  int numPoints = width * height;
  std::vector<Point> points;
  
  for(int i = 0; i < numPoints; i++) {
    Point p;
    p.x[0] = in[i * 3 + 0];
    p.x[1] = in[i * 3 + 1];
    p.x[2] = in[i * 3 + 2];
    points.push_back(p);
  }
  
  std::list<Point> palette = medianCut(&points[0], numPoints, K);
  std::list<Point>::iterator iter;
  int i = 0;
  for (iter = palette.begin() ; iter != palette.end(); iter++) {
    dico[0][i] = (int)iter->x[0];
    dico[1][i] = (int)iter->x[1];
    dico[2][i] = (int)iter->x[2];
    i++;
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

      at(out, width, height, i, j, Yc) = fmax(0, fmin(255, y));;
      at(out, width, height, i, j, Cr) = fmax(0, fmin(255, u));;
      at(out, width, height, i, j, Cb) = fmax(0, fmin(255, v));;
    }
  }
  return out;
}


OCTET* toRGB(OCTET* in, int width, int height) {
  OCTET *out;
  allocation_tableau(out, OCTET, width * height * 3);

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {

      double y = at(in, width, height, i, j, Yc);
      double u = at(in, width, height, i, j, Cr) - 128;
      double v = at(in, width, height, i, j, Cb) - 128;

      double red = y + 1.13983 * v;
      double green = y - 0.39465 * u - 0.58060 * v;
      double blue = y + 2.03211 * u;

      // if(red > 255 || red < 0) std::cout << "red = " << red << std::endl;
      // if(green > 255 || green < 0) std::cout << "green = " << green << std::endl;
      // if(blue > 255 || blue < 0) std::cout << "blue = " << blue << std::endl;

      // if(red < -10) red = 255 - red;

      at(out, width, height, i, j, RED) = fmax(0, fmin(255, red));
      at(out, width, height, i, j, GREEN) = fmax(0, fmin(255, green));
      at(out, width, height, i, j, BLUE) = fmax(0, fmin(255, blue));

    }
  }
  return out;
}



OCTET *encodeFromDico(std::vector<std::vector<OCTET> > dico, OCTET *in, int width, int height) {
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

void writeDicoToStream(std::vector<std::vector<OCTET> > dico, OCTET* in, int width, int height, std::ostream& stream, int dicoValueSize) {
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

  stream << (unsigned char)dicoValueSize;
  short w = width, h = height;
  stream << (unsigned char) (w / 256);
  stream << (unsigned char) (w % 256);
  stream << (unsigned char) (h / 256);
  stream << (unsigned char) (h % 256);
  
  for(auto vec : dico) {
    for(auto i : vec) {
      stream << i;
    }
  }

  OCTET last = 0;
  OCTET count = 0;
  
  for (int i = 0; i < width * height; i++) {
    auto value = getValue(i);

    if(count == 255) {
      stream << count;
      stream << last;
      count = 1;
    } else if(count == 0) {
      last = value;
      count = 1;
    } else {
      if(last != value) {
	stream << count;
	stream << last;
	count = 1;
	last = value;
      } else {
	count++;
      }
    }
  }
  stream.flush();
}

std::vector<OCTET> decodeFromDico(std::ifstream& stream, int& width, int& height) {
  int size = stream.get();
  width = stream.get() * 256 + stream.get();
  height = stream.get() * 256 + stream.get();

  std::cout << "width = " << width << std::endl;
  std::cout << "height = " << height << std::endl;
  std::vector<std::vector<unsigned char> > dico;

  std::cout << "size = " << size << std::endl;

  for (int i = 0; i < 3; i++) {
    std::vector<unsigned char> vec;
    for (int j = 0; j < pow(2, size); j++) {
      vec.push_back(stream.get());
    }    
    dico.push_back(vec);
  }
  
  std::vector<unsigned char> out;

  while(stream.peek() != EOF) {

    unsigned char count = stream.get();
    unsigned char value = stream.get();

    for (int i = 0; i < count; i++) {
      out.push_back(dico[0][value]);
      out.push_back(dico[1][value]);
      out.push_back(dico[2][value]);
    }
  }

   return out;
}


std::vector<OCTET> resizeImageChannel(OCTET* in, int width, int height, int dW, int dH, int color) {
  std::vector<OCTET> temp;
  std::vector<OCTET> out;

  for (int i = 0; i < width * height * 3; i++) {
    out.push_back(in[i]);
  }

  auto setAreaToAvgOfArea = [=](std::vector<OCTET>in, int x, int y, int w, int h) {
    long avg = 0;
    for (int j = y; j < h + y && j < height; j++) {
      for (int i = x; i < w + x && i < width; i++) {
	avg += at(&in[0], width, height, i, j, 0);
	// avg += in[i * height + j];
      }
    }
    avg /= w * h;
    for (int j = y; j < h + y && j < height; j++) {
      for (int i = x; i < w + x && i < width; i++) {
	at(&in[0], width, height, i, j, 0) = avg;
      }
    }
    return in;
  };

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      temp.push_back(at(in, width, height, i, j, color));
    }
  }

  for (int j = 0; j < height; j += dH) {
    for (int i = 0; i < width; i += dW) {
      temp = setAreaToAvgOfArea(temp, i, j, dW, dH);
    }
  }

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      at(&out[0], width, height, i, j, color) = at(&temp[0], width, height, i, j, 0);
    }
  }

  return out;
}
