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

#include "Vector3f.h"

class Drawable {
public:
  virtual void draw() = 0;
};

class Screen {
public:
  // Load the .obj file and override the faces stored
  void addObjNode(Drawable *objNode);

  // Render the models in screen
  void drawScreen();

private:
  // attributes
  std::vector<Drawable *> childNodes;
};

#endif