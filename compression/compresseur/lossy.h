#pragma once

#include <vector>
#include <stack>
#include <assert.h>
#include <iostream>
#include <fstream>

#include "kmeans.h"
#include "median_cut.h"
#include "image_ppm.h"
#include "lossless.h"

std::vector<std::vector<OCTET> > extractDico(OCTET *in, int width, int height, int dicoSize = 8);
std::vector<std::vector<OCTET> > extractDicoKmeans(OCTET *in, int width, int height, int dicoSize = 8);
std::vector<std::vector<OCTET> > extractDicoMediaCut(OCTET *in, int width, int height, int dicoSize = 8);
OCTET *encodeFromDico(std::vector<std::vector<OCTET> > dico, OCTET *in, int width, int height);
OCTET *toYCbCr(OCTET* in, int width, int height);
OCTET *toRGB(OCTET* in, int width, int height);
void writeDicoToStream(std::vector<std::vector<OCTET> > dico, OCTET* encodedImage, int width, int height, std::ostream& stream, int dicoValueSize);
std::vector<OCTET> decodeFromDico(std::ifstream& stream, int& width, int& height);
std::vector<OCTET> resizeImageChannel(OCTET* in, int width, int height, int dW, int outHeight, int color);
