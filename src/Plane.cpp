#include <iostream>
#include <cstdlib>

#include "Plane.h"

using namespace std;

Plane::Plane(const float startPos, const float endPos, const float planePos,
             const Vector2f planeNormal) {
  this->normal = planeNormal.normalized();

  // validate parameters
  if (this->normal.x() == 0.0f && this->normal.y() == 0.0f) {
    cerr << "Plane normal vector cannot be a point." << endl;
    exit(1);
  }

  if (startPos == endPos) {
    cerr << "Plane min & max position cannot be equal." << endl;
    exit(1);
  } else if (startPos > endPos) {
    cerr << "Plane min position cannot be greater than max position." << endl;
    exit(1);
  }

  bool yPlane = this->normal.x() != 0 ? true : false;
  if (yPlane) {
    this->yMin = startPos;
    this->yMax = endPos;
    this->xMin = this->xMax = planePos;
    this->center = Vector2f(planePos, startPos + (endPos - startPos) / 2.0f);
  } else {
    this->xMin = startPos;
    this->xMax = endPos;
    this->yMin = this->yMax = planePos;
    this->center = Vector2f(startPos + (endPos - startPos) / 2.0f, planePos);
  }
  this->planePos = planePos;
}

float Plane::getSizeX() {
  if (this->normal.x() != 0) { // Y plane
    return 0.0f;
  } else { // X plane
    return abs(this->xMax - this->xMin);
  }
}

float Plane::getSizeY() {
  if (this->normal.x() != 0) { // Y plane
    return abs(this->yMax - this->yMin);
  } else { // X plane
    return 0.0f;
  }
}

Vector2f clamp(Vector2f value, Vector2f minVec, Vector2f maxVec) {
  return Vector2f(max(minVec.x(), min(maxVec.x(), value.x())),
                  max(minVec.y(), min(maxVec.y(), value.y())));
}

bool Plane::collide(const Ray *incomingRay, Vector2f &difference) {
  Vector2f halfExtend = Vector2f(this->getSizeX() / 2.0f, this->getSizeY() / 2.0f);
  Vector2f tempDifference = incomingRay->getOrigin() - this->center;
  Vector2f clamped = clamp(tempDifference, -halfExtend, halfExtend);
  Vector2f closest = this->center + clamped;
  difference = closest - incomingRay->getOrigin();

  return difference.abs() < incomingRay->getDistance();
}
