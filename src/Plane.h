#ifndef PLANE_H
#define PLANE_H

#include "Physics.h"
#include "Ray.h"

class Collidable {
  virtual bool collide(Ray incomingRay) = 0;
};

struct PlaneIndex {
  const float planePos, minPos, maxPos;

  bool const operator==(const PlaneIndex &pIndex) const {
    return planePos == pIndex.planePos && minPos == pIndex.minPos && maxPos == pIndex.maxPos;
  }

  bool const operator<(const PlaneIndex &pIndex) const {
    return planePos < pIndex.planePos && minPos < pIndex.minPos && maxPos < pIndex.maxPos;
  }

  bool const operator>(const PlaneIndex &pIndex) const {
    return planePos > pIndex.planePos && minPos > pIndex.minPos && maxPos > pIndex.maxPos;
  }
};

class Plane : public Collidable {
public:
  Plane(float startPos, float endPos, float planePos, Vector2f normal);

  // Check if the object will collide with the incoming ray
  bool collide(Ray incomingRay);

  Vector2f normal;
  float xMin, xMax, yMin, yMax;
};
#endif
