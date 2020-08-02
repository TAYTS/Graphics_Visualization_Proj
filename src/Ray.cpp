#include "Ray.h"

Ray::Ray(const Vector2f &origin, const Vector2f &direction, const float distance) {
  this->origin = origin;
  this->direction = direction.normalized();
  this->distance = distance;
}

Ray::Ray(const Ray &r) {
  this->origin = r.origin;
  this->direction = r.direction.normalized();
  this->distance = r.distance;
}

void Ray::set(Vector2f &origin, Vector2f &direction, float distance) {
  this->origin = origin;
  this->direction = direction.normalized();
  this->distance = distance;
}

const Vector2f &Ray::getOrigin() const { return this->origin; }

const Vector2f &Ray::getDirection() const { return this->direction; }

Vector2f Ray::getEndpoint() const { return this->origin + this->direction * this->distance; }
