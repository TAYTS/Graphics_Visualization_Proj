#include <iostream>

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

  bool xPlane = this->normal.x() != 0 ? true : false;
  if (xPlane) {
    this->yMin = startPos;
    this->yMax = endPos;
    this->xMin = this->xMax = planePos;
  } else {
    this->xMin = startPos;
    this->xMax = endPos;
    this->yMin = this->yMax = planePos;
  }
}

bool Plane::collide(Ray incomingRay) {
  // Only consider incoming ray that is in the opposite direction
  // of the plane normal
  if ((this->normal.x() > 0.0f && incomingRay.getDirection().x() > 0.0f) ||
      (this->normal.x() < 0.0f && incomingRay.getDirection().x() < 0.0f) ||
      (this->normal.y() > 0.0f && incomingRay.getDirection().y() > 0.0f) ||
      (this->normal.y() < 0.0f && incomingRay.getDirection().y() < 0.0f)) {
    return false;
  }

  Vector2f origin = incomingRay.getOrigin();
  Vector2f finalPosition = incomingRay.getEndpoint();
  // check if the incoming ray cross the plane and its
  // final position is within the plane range
  if (this->normal.x() != 0.0f) { // y-plane
    return finalPosition.x() <= this->xMin <= origin.x() && finalPosition.y() >= this->yMin &&
           finalPosition.y() <= this->yMax;
  } else if (this->normal.y() != 0.0f) { // x-plane
    return finalPosition.y() <= this->yMin <= origin.y() && finalPosition.x() >= this->xMin &&
           finalPosition.x() <= this->xMax;
  }

  return false;
}
