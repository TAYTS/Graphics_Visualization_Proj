#ifndef BALL_H
#define BALL_H

#include "Vector3f.h"
#include "Matrix4f.h"
#include "Screen.h"

class Ball : public Drawable {
public:
  Ball(float radius);
  Ball(float radius, Vector3f center);
  Ball(float radius, Vector3f center, Vector3f velocity);

  void pushTransMatrix(Matrix4f transMat);

  void updateNextPosition();

  // Render the model
  void draw();

  // attributes
  float radius;
  Vector3f center, velocity;
  Matrix4f transMat = Matrix4f::identity();
  const Vector3f GRAVITY = Vector3f(0.0f, -9.81f, 0.0f);
};

#endif