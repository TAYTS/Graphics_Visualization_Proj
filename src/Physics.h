#ifndef PHYSICS_H
#define PHYSICS_H

#include "Hit.h"
#include "Ray.h"

class Physics {
public:
  virtual bool collide(Ray &r, Hit &hit, float radius) = 0;

  void setGravity(float g);

  float getGravity();

private:
  float gravity = 0;
};

#endif // PHYSICS_H
