#ifndef RAY_H
#define RAY_H

#include <ostream>
#include "Vector2f.h"

class Ray {
public:
  Ray(const Vector2f &origin, const Vector2f &direction, const float distance);

  Ray(const Ray &r);

  void set(Vector2f &origin, Vector2f &direction, float distance);

  const Vector2f &getOrigin() const;

  const Vector2f &getDirection() const;

  const float &getDistance() const;

  Vector2f getEndpoint() const;

private:
  Vector2f origin;
  Vector2f direction; // Normalized vector
  float distance;
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
  os << "Ray <" << r.getOrigin() << ", " << r.getDirection() << ">";
  return os;
}

#endif // RAY_H
