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

////////////////////////////////////
//        Global Variables        //
////////////////////////////////////
#define IMG_WIDTH 128
#define IMG_HEIGHT 128

// Texture related

GLubyte *readBmpTex(string bmpFile) {
  // reads a bitmap file with 8 bit per colour as texture data
  //bitmap size is controlled by IMG_WIDTH and IMG_HEIGHT, which must be a power of 2 e.g. 32,64,128,256
  ifstream texFile;
  uint32_t fileDataOffset=0;

  texFile.open(bmpFile, ios_base::binary);
  char input[2];
  texFile.read(input, 2);
  if (strcmp(input,"BM")==0) {
    cerr << "texture file is not of supported file format";
    string temp;
    cin >> temp;
    exit(2);
  }
char inputByte;
  texFile.seekg(0x0A);
  texFile.get(inputByte);
  fileDataOffset += (unsigned char)inputByte;
  texFile.get(inputByte);
  fileDataOffset += (uint32_t)((unsigned char)inputByte) << 8;
  texFile.get(inputByte);
  fileDataOffset += (uint32_t)((unsigned char)inputByte) << 16;
  texFile.get(inputByte);
  fileDataOffset += (uint32_t)((unsigned char)inputByte) << 24;

  texFile.seekg(0x12);
  texFile.get(inputByte);
  uint32_t  texWidth = (unsigned char)inputByte;
  texFile.get(inputByte);
  texWidth += (uint32_t)((unsigned char)inputByte) << 8;
  texFile.get(inputByte);
  texWidth += (uint32_t)((unsigned char)inputByte) << 16;
  texFile.get(inputByte);
  texWidth += (uint32_t)((unsigned char)inputByte) << 24;

  texFile.get(inputByte);
  uint32_t texHeight = (unsigned char)inputByte;
  texFile.get(inputByte);
  texHeight += (uint32_t)((unsigned char)inputByte) << 8;
  texFile.get(inputByte);
  texHeight += (uint32_t)((unsigned char)inputByte) << 16;
  texFile.get(inputByte);
  texHeight += (uint32_t)((unsigned char)inputByte) << 24;

  if (texWidth != IMG_WIDTH || texHeight != IMG_HEIGHT) {
    cout << dec<<"width: " << texWidth<<"\n";
    cout << "height: " << texHeight <<"\n";
    cerr << "texture file is not " << IMG_WIDTH << " bit by " << IMG_HEIGHT
         << " bit, please modify\n";
    string temp;
    cin >> temp;
    exit(3);
  }

  texFile.seekg(0x1C);
  texFile.get(inputByte);
  uint16_t bitPerPixel = (unsigned char)inputByte;
  texFile.get(inputByte);
  bitPerPixel += (uint32_t)((unsigned char)inputByte) << 8;

  if (bitPerPixel != 24) {
    cout << "detected bit per pixel: "<<dec<<bitPerPixel<<"\n";
    cerr << "texture file is not 8 bit per color, please modify";
    string temp;
    cin >> temp;
    exit(3);
  }

  texFile.seekg(fileDataOffset);
  static GLubyte imgTexture[IMG_HEIGHT][IMG_WIDTH][3];
  for (int i = 0; i < texWidth; i++) {
    for (int j = 0; j < texHeight; j++) {
      char col;
      texFile.get(col);
      imgTexture[i][j][2] = (unsigned char)col;
      texFile.get(col);
      imgTexture[i][j][1] = (unsigned char)col;
      texFile.get(col);
      imgTexture[i][j][0] = (unsigned char)col;
    }
  }
  return reinterpret_cast<GLubyte *>(imgTexture);
}

GLuint initTexture(string bmpFile) {
  /* create texture object, texture environment and
     read texture from 128x128 pixel bitmap file with 8 bit per colour */
  GLuint texInt;
  glGenTextures(1, &texInt);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

  GLubyte *texData = readBmpTex(bmpFile);
  glBindTexture(GL_TEXTURE_2D, texInt);
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, IMG_WIDTH, IMG_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE,
                    texData);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  return texInt;
}

void finishTexture(int texCount, GLuint* texObj) {
  /* disable texturing and unbind texture */
  glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(4, texObj);
}

void finishTexture(GLuint texObj) {
  /* disable texturing and unbind texture */
  glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &texObj);
}

void DrawTextureQuad(GLuint texObj) {
    // used to check proper loading of texture
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texObj);

  glBegin(GL_QUADS);
  {
    glTexCoord2d(0.0, 0.0);
    glVertex2d(-12.5, -12.5);
    glTexCoord2d(5.0, 0.0);
    glVertex2d(12.5, -12.5);
    glTexCoord2d(5.0, 5.0);
    glVertex2d(12.5, 12.5);
    glTexCoord2d(0.0, 5.0);
    glVertex2d(-12.5, 12.5);
  }
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}