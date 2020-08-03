#ifndef TEXTURE_H
#define TEXTURE_H

#include <string.h>

#include "Screen.h"

// Load texture image file
GLubyte *readBmpTex(std::string bmpFile);

// Create texture object, texture environment and read texture
// from 128x128 pixel bitmap file with 8 bit per colour
GLuint initTexture(std::string bmpFile);

#endif