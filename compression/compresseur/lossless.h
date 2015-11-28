#pragma once

#include <vector>
#include <stack>
#include <assert.h>
#include <iostream>
#include <fstream>

#include "image_ppm.h"

void writeRLEToFile(OCTET* encodedImage, int width, int height, std::string path);
