#ifndef MAZE_H
#define MAZE_H
#include <iostream>
#include <string>
#include <vector>

#include "Vector3f.h"

struct Face {
  const unsigned int v1, v2, v3, n1, n2, n3;
};

class Maze {
public:
  Maze();
  Maze(std::string objFilePath);

  // Load the .obj file and override the faces stored
  void loadObj(std::string objFilePath);

  // Render the model
  void renderModel();

  // Get model overall size in X axis
  float getSizeX();
  // Get model overall size in Y axis
  float getSizeY();
  // Get model overall size in Z axis
  float getSizeZ();

  // attributes
  float minX = 0.0f, minY = 0.0f, minZ = 0.0f;
  float maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;
  std::vector<Vector3f> vectices;
  std::vector<Vector3f> normals;
  std::vector<Face> faces;
};

#endif