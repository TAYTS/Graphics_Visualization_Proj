#include <iostream>

#include "Ball.h"
#include "Matrix4f.h"

using namespace std;

Ball::Ball(float radius) {
  this->radius = radius;
  this->center = Vector3f::ZERO;
  this->velocity = Vector3f::ZERO;
};

Ball::Ball(float radius, Vector3f center) : Ball(radius) { this->center = center; };

Ball::Ball(float radius, Vector3f center, Vector3f velocity) : Ball(radius, center) {
  this->velocity = velocity;
}

void Ball::setNewCenter(Vector3f center) { this->center = center; }

void Ball::draw() {
  glPushMatrix();
  glTranslated(this->center.x(), this->center.y(), this->center.z() + this->radius);
  glutSolidSphere(this->radius, 12, 12);
  glPopMatrix();
}