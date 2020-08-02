#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#elif defined _WIN32 || defined _WIN64
#include <glut.h>
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

GLubyte *readBmpTex(string bmpFile);

GLuint initTexture(string bmpFile);

void finishTexture(int texCount, GLuint *texObj);

void finishTexture(GLuint texObj);

void DrawTextureQuad(GLuint texObj);
#endif