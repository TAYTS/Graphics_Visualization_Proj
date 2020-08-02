#ifndef PLANE_H
#define PLANE_H
#include <iostream>

#include "Ray.h"

using namespace std;
class Collidable {
  virtual bool collide(Ray incomingRay) = 0;
};

struct PlaneIndex {
  const float planePos, minPos, maxPos;

  bool const operator==(const PlaneIndex &pIndex) const {
    return planePos == pIndex.planePos && minPos == pIndex.minPos && maxPos == pIndex.maxPos;
  }

  bool const operator<(const PlaneIndex &pIndex) const {
    if (planePos < pIndex.planePos) {
      return true;
    } else if (planePos == pIndex.planePos) {
      if (minPos < pIndex.minPos) {
        return true;
      } else if (minPos == pIndex.minPos) {
        return maxPos < pIndex.maxPos;
      }
    }
    return false;
  }
};

class Plane : public Collidable {
public:
  Plane(float startPos, float endPos, float planePos, Vector2f normal);

  float getSizeX();
  float getSizeY();

  // Check if the object will collide with the incoming ray
  bool collide(Ray incomingRay);

  Vector2f normal, center;
  float xMin, xMax, yMin, yMax, planePos;
};
#endif
