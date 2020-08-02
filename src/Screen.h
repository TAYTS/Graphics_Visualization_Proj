#ifndef SCREEN_H
#define SCREEN_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#elif defined _WIN32 || defined _WIN64
#include <glut.h>
#endif

#include <string>
#include <vector>
#include <map>

#include "Vector3f.h"

#define _ZNEAR 1.0
#define _ZFAR 100.0
#define _ROTSCALE 0.5
#define FOV 60.0
#define REFRESH_RATE 1000.0 / 60.0 // 60Hz
#define GRAVITY 9.81

class Drawable {
public:
  virtual void draw() = 0;
};

typedef std::map<std::string, Drawable *> ObjMap;
class Screen {
public:
  // Load the .obj file and override the faces stored
  void addObjNode(std::string objKey, Drawable *objNode);

  // Render the models in screen
  void drawScreen();

  Drawable *getObject(std::string objKey);

  // Mouse interaction attributes
  GLint mouseModifiers = 0;
  GLint mouseButton = 0;
  int prevMouseX, prevMouseY;

  // attributes
  ObjMap childNodes;
};

#endif