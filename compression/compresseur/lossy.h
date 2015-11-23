#pragma once

#include <vector>

#include "image_ppm.h"

//extrait le dictionnaire le couleurs
std::vector<std::vector<OCTET>> extractDico(OCTET *in, int width, int height, int dicoSize = 8);
OCTET *encodeFromDico(std::vector<std::vector<OCTET>> dico, OCTET *in, int width, int height);
OCTET *toYCbCr(OCTET* in, int width, int height);
OCTET *toRGB(OCTET* in, int width, int height);
