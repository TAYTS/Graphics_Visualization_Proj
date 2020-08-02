#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

GLubyte *readBmpTex(string bmpFile);

GLuint initTexture(string bmpFile);

void finishTexture(int texCount, GLuint *texObj);

void finishTexture(GLuint texObj);

void DrawTextureQuad(GLuint texObj);
#endif