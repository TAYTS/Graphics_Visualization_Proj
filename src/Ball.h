#ifndef BALL_H
#define BALL_H

#include "Vector3f.h"
#include "Screen.h"

class Ball : public Drawable {
public:
  Ball(float radius);
  Ball(float radius, Vector3f center);
  Ball(float radius, Vector3f center, Vector3f velocity);

  void setNewCenter(Vector3f center);

  // Render the model
  void draw();

  // attributes
  float radius;
  Vector3f center = Vector3f::ZERO, velocity = Vector3f::ZERO;
};

#endif