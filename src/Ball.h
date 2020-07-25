#ifndef BALL_H
#define BALL_H

#include <vector>

#include "Vector3f.h"
#include "Matrix4f.h"
#include "Screen.h"
#include "Physics.h"
#include "vecmath.h"
#include "Ray.h"
#include "Plane.h"

class Ball : public Drawable, public Physics {
public:
  Ball(float radius);
  Ball(float radius, float COR, Vector3f center);

  ~Ball() {}

  void pushTransMatrix(Matrix4f transMat);

  bool collide(Ray &r, Hit &h, float radius);

  void addCollidable(Plane collisionItem) { this->collidable.push_back(collisionItem); }

  void computeNextPos(float intervalTime);

  // Render the model
  void draw();

  // attributes
  float radius;
  Vector3f center, velocity;
  Matrix4f transMat = Matrix4f::identity();
  Matrix4f globalToObjectTransMatrix = Matrix4f::identity();
  Matrix4f objectToGlobalTransMatrix = Matrix4f::identity();
  const Vector3f GRAVITY = Vector3f(0.0f, -9.81f, 0.0f);
  float coefficientofRestituition = 1.0f;
  vector<Plane> collidable;
};

#endif
