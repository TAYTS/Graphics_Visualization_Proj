#ifndef MAZE_H
#define MAZE_H
#include <string>
#include <vector>

#include "Vector2f.h"
#include "Vector3f.h"
#include "Screen.h"
#include "Plane.h"

struct Face {
  const unsigned int v1, v2, v3, n1, n2, n3;
  Face(const unsigned int v1, const unsigned int v2, const unsigned int v3, const unsigned int n1,
       const unsigned int n2, const unsigned int n3)
      : v1(v1), v2(v2), v3(v3), n1(n1), n2(n2), n3(n3) {}
  float t1_x = 0.0f, t1_y = 0.0f, t2_x = 0.0f, t2_y = 0.0f, t3_x = 0.0f, t3_y = 0.0f;
  GLuint texture = 0;
};

typedef std::map<PlaneIndex, Plane> PlaneMap;

class Maze : public Drawable {
public:
  Maze();
  Maze(std::string objFilePath, std::string mazeTopTexturePath, std::string floorTexturePath,
       std::string wallTexturePath);

  // Load the .obj file and override the faces stored
  void loadObj(std::string objFilePath);

  // Load the texture files and override the textures stored in faces
  void loadTexture(std::string mazeTopTexturePath, std::string floorTexturePath,
                   std::string wallTexturePath);

  // Render the model
  void draw();

  // Get model overall size in X axis
  float getSizeX();
  // Get model overall size in Y axis
  float getSizeY();
  // Get model overall size in Z axis
  float getSizeZ();

  // Get the maze starting (edge) position
  Vector2f getMazeStartPos();

  // Get the maze ending (edge) position
  Vector2f getMazeEndPos();

  // Get all the planes that will collide with the given Ray
  vector<pair<PlaneIndex, Plane> *> getPotentialCollidePlanePair(Ray &ray);

  // attributes
  float minX = 0.0f, minY = 0.0f, minZ = 0.0f;
  float maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;
  std::vector<Vector3f> vectices;
  std::vector<Vector3f> normals;
  std::vector<Face> faces;
  vector<pair<PlaneIndex, Plane>> posFacingXPlane, negFacingXPlane, posFacingYPlane,
      negFacingYPlane;

private:
  int binarySearchPlane(Ray &ray, vector<pair<PlaneIndex, Plane>> &planeTable, bool xPlane) const;
};

#endif