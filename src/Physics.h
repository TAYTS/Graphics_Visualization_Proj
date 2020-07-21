#ifndef PHYSICS_H
#define PHYSICS_H

#include "Hit.h"
#include "Ray.h"

class Physics {
public:
  virtual bool collide(Ray &r, Hit &hit, float radius) = 0;
  
  void setGravity(float g) { this->gravity = g; 
  }
  
  float getGravity() { return this->gravity;
  }

private:
  float gravity=0;
  
};

#endif // PHYSICS_H
