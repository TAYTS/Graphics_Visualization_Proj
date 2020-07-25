#include "Physics.h"

#include "Hit.h"
#include "Ray.h"

void Physics::setGravity(float g) { this->gravity = g; }

float Physics::getGravity() { return this->gravity; }
