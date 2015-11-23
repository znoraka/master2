#pragma once

#include <vector>

#include "kmeans.h"
#include "image_ppm.h"

struct color_point
{
  OCTET r;
  OCTET g;
  OCTET b;
};

struct sum_point
{
  long r;
  long g;
  long b;
};

struct count_point
{
  int r;
  int g;
  int b;
};

std::vector<std::vector<OCTET>> extractDico(OCTET *in, int width, int height, int dicoSize = 8);
std::vector<std::vector<OCTET>> extractDicoKmeans(OCTET *in, int width, int height, int dicoSize = 8);
OCTET *encodeFromDico(std::vector<std::vector<OCTET>> dico, OCTET *in, int width, int height);
OCTET *toYCbCr(OCTET* in, int width, int height);
OCTET *toRGB(OCTET* in, int width, int height);
