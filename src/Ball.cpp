#include <iostream>
#include <algorithm>

#include "Ball.h"
#include "Screen.h"
#include "Ray.h"

using namespace std;

Ball::Ball(Maze *maze, float radius) {
  this->maze = maze;
  this->radius = radius;
  this->COR = 1.0f;
  this->center = Vector2f::ZERO;
  this->velocity = Vector2f::ZERO;
  this->counterVelocity = this->velocity;
  this->transMat =
      Matrix4f::translation(0.0f, 0.0f, this->radius); // Set the ball to be on top of Maze plane
  this->movementLimit = pair<Vector2f, Vector2f>(Vector2f::ZERO, Vector2f::ZERO);
};

Ball::Ball(Maze *maze, float radius, float COR, Vector2f center,
           pair<Vector2f, Vector2f> movementLimit)
    : Ball(maze, radius) {
  this->COR = COR;
  this->center = center;
  this->movementLimit = movementLimit;
};

void Ball::draw() {
  // Store the old rendering settings
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // Set line color for render the gravity force line
  // change ball to different color
  GLfloat diffuseColor[4] = {1.0, 0.0, 0.0, 1.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);

  glPushMatrix();
  // Offset the model view origin to rende the ball
  glTranslated(this->center.x(), this->center.y(), this->radius);
  glutSolidSphere(this->radius, 12, 12);

  glPopMatrix();

  // Restore the old rendering settings
  glPopAttrib();

  this->computeNextPos();
}

// void Ball::computeNextPos(float intervalTime) {
//   Vector3f accDir = (this->globalToObjectTransMatrix * Vector4f(0.0f, -1.0f, 0.0f, 0.0f)).xyz();
//   Vector2f acc = this->getGravity() * accDir.normalized().xy();
//   Vector2f dir = intervalTime * this->velocity.xy() + 0.5 * intervalTime * intervalTime * acc;
//   Ray ballPath(this->center.xy(), dir);
//   Hit pathHit;
//   do {
//     bool collide = false;
//     for (int i = 0; i < this->collidable.size(); i++) {
//       collide = collide || this->collidable[i].collide(ballPath, pathHit, this->radius);
//     }
//     if (collide && pathHit.getT() < 1) {
//       Vector2f dist = dir * pathHit.getT();
//       this->center += Vector3f(dist, 0.0f);
//       Vector2f determinant =
//           (this->velocity * this->velocity + Vector3f(2 * acc * dist, 0.0f)).xy();
//       int temp = abs(acc[0]) == 0.0f ? 1 : 0;
//       float timeTaken1 = 0 - pow(determinant[temp], 0.5) - velocity[temp];
//       timeTaken1 = timeTaken1 / acc[temp];
//       float timeTaken2 = pow(determinant[temp], 0.5) - velocity[temp];
//       timeTaken2 = timeTaken2 / acc[temp];
//       float timeTaken = timeTaken1;
//       if (timeTaken1 < 0.0f) {
//         timeTaken = timeTaken2;
//       } else if (timeTaken2 < timeTaken1 && timeTaken2 >= 0.0f) {
//         timeTaken = timeTaken2;
//       }
//       this->velocity = this->velocity + Vector3f(acc * timeTaken, 0.0f);
//       float speed = this->velocity.abs();
//       this->velocity.negate();
//       // this->velocity.print();
//       this->velocity.normalize();
//       this->velocity =
//           2 * pathHit.getNormal() * Vector2f::dot(pathHit.getNormal(), this->velocity.xy()) -
//           this->velocity;
//       // this->velocity.print();
//       this->velocity *= this->coefficientofRestituition * speed;
//       // this->velocity.print();
//       if (dist.abs() < 1e-21 && speed < 1e-21) {
//         dir = Vector2f::ZERO;
//         acc = Vector2f::ZERO;
//         this->velocity = Vector3f::ZERO;
//         break;
//       }
//       intervalTime -= timeTaken;
//       pathHit.reset();
//       dir = this->velocity.xy() * intervalTime + 0.5 * acc * intervalTime * intervalTime;
//       Vector2f tempCenter = this->center.xy();
//       ballPath.set(tempCenter, dir);
//     } else {
//       break;
//     }
//     // cout <<"interavalTime:"<< intervalTime<<"\n";
//   } while (intervalTime > 0.0f);
//   this->center += Vector3f(dir, 0.0f);
//   this->velocity += Vector3f(acc * intervalTime, 0.0f);
// }

Vector2f clamp1(Vector2f value, Vector2f minVec, Vector2f maxVec) {
  return Vector2f(max(minVec.x(), min(maxVec.x(), value.x())),
                  max(minVec.y(), min(maxVec.y(), value.y())));
}

void Ball::computeNextPos() {
  Matrix4f objToWorldMatrix = this->getObjToWorldMatrix();
  Matrix4f worldToObjMatrix = this->getWorldToObjMatrix();

  bool collide = false;
  float timestep = REFRESH_RATE / 520.0f;

  Vector3f gravityAccel = Vector3f(0.0, 0.0, -GRAVITY); // gravity force point into the screen
  // Maze normal in world coordinate system
  Vector3f mazeNormal = (objToWorldMatrix * Vector4f(-Vector3f::FORWARD, 0.0f)).xyz().normalized();

  // do {
  // Compute the projected gravity vector in world coordinate system
  Vector3f projGravityVec = gravityAccel - (Vector3f::dot(gravityAccel, mazeNormal)) * mazeNormal;
  // Convert the projected gravity vector to local coordinate system
  projGravityVec = (worldToObjMatrix * Vector4f(projGravityVec, 0.0f)).xyz();

  Vector2f newVelocity = this->velocity;
  if (projGravityVec.xy() != Vector2f::ZERO) {
    // v1 = v0 + a * time_delta
    newVelocity = this->velocity + (projGravityVec.xy() * timestep);
  } else {
    // friction
    if (newVelocity.x() != 0) {
      newVelocity -= Vector2f(newVelocity.x() * 0.15f, 0.0f);
    }
    if (newVelocity.y() != 0) {
      newVelocity -= Vector2f(0.0f, newVelocity.y() * 0.15f);
    }
  }

  // formula: delta_distance = 0.5 * (v1 + v0) * time_delta
  Vector2f totalDisplacement = 0.5 * (this->velocity + newVelocity) * timestep;
  Vector2f displacement = Vector2f::ZERO;
  Vector2f newCenter = this->center + totalDisplacement;

  // check if the new center exit the maze boundary
  if (newCenter.y() + this->radius >= this->movementLimit.first.y()) {
    newVelocity = (newVelocity * this->COR);
    this->velocity = Vector2f(newVelocity.x(), newVelocity.y() * -1.0f);
    this->center = Vector2f(newCenter.x(), this->movementLimit.first.y() - this->radius);
    return;
  } else if (newCenter.y() - this->radius <= this->movementLimit.second.y()) {
    newVelocity = (newVelocity * this->COR);
    this->velocity = Vector2f(newVelocity.x(), newVelocity.y() * -1.0f);
    this->center = Vector2f(newCenter.x(), this->movementLimit.second.y() + this->radius);
    return;
  }

  vector<Vector2f> dirs = {
      Vector2f(0.0f, 1.0f),
      Vector2f(0.0f, -1.0f),
      Vector2f(-1.0f, 0.0f),
      Vector2f(1.0f, 0.0f),
  };

  for (auto dir : dirs) {
    Ray ballDirection = Ray(newCenter, dir, this->radius);

    vector<pair<PlaneIndex, Plane> *> potentialCollidePlane =
        this->maze->getPotentialCollidePlanePair(ballDirection);

    if (potentialCollidePlane.size() > 0) {
      for (auto planePair : potentialCollidePlane) {
        Vector2f halfExtend =
            Vector2f(planePair->second.getSizeX() / 2.0f, planePair->second.getSizeY() / 2.0f);
        Vector2f difference1 = newCenter - planePair->second.center;
        Vector2f clamped = clamp1(difference1, -halfExtend, halfExtend);
        Vector2f closest = planePair->second.center + clamped;
        difference1 = closest - newCenter;

        Vector2f difference = Vector2f::ZERO;
        collide = planePair->second.collide(&ballDirection, difference);

        if (collide) {
          if (planePair->second.normal.x() != 0.0f) { // Y plane
            float penetration = this->radius - abs(difference.x());
            penetration = floor(penetration * 1000000.0f) / 1000000.0f;

            // flip the vector direction
            if ((planePair->second.normal.x() > 0 && newVelocity.x() < 0) ||
                (planePair->second.normal.x() < 0 && newVelocity.x() > 0)) {
              newVelocity.x() *= (-1.0f * this->COR);
            }
            if (planePair->second.normal.x() > 0.0f) {
              newCenter.x() += penetration;
            } else {
              newCenter.x() -= penetration;
            }
          } else { // X plane
            float penetration = this->radius - abs(difference.y());
            penetration = floor(penetration * 1000000.0f) / 1000000.0f;

            if ((planePair->second.normal.y() > 0 && newVelocity.y() < 0) ||
                (planePair->second.normal.y() < 0 && newVelocity.y() > 0)) {
              newVelocity.y() *= (-1.0f * this->COR);
            }

            if (planePair->second.normal.y() > 0.0f) {
              newCenter.y() += penetration;
            } else {
              newCenter.y() -= penetration;
            }
          }
        }
      }
    }
    if (collide) {
      break;
    }
  }

  this->velocity = newVelocity;
  this->center = newCenter;
  /* Rendering vector */
  // glPushAttrib(GL_ALL_ATTRIB_BITS);
  // glDisable(GL_LIGHTING);
  // glLineWidth(5);

  // glPushMatrix();

  // glBegin(GL_LINES);
  // glColor4f(0, 1, 0, 0);
  // glVertex3fv(Vector3f(this->center, this->radius));
  // glVertex3fv(Vector3f(this->center + this->velocity, this->radius));
  // glEnd();

  // glPopMatrix();
  // glPopAttrib();
}

Matrix4f Ball::getObjToWorldMatrix() {
  float matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  return Matrix4f(matrix[0], matrix[4], matrix[8], matrix[12], matrix[1], matrix[5], matrix[9],
                  matrix[13], matrix[2], matrix[6], matrix[10], matrix[14], matrix[3], matrix[7],
                  matrix[11], matrix[15]);
}

Matrix4f Ball::getWorldToObjMatrix() {
  float matrix[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

  return Matrix4f(matrix[0], matrix[4], matrix[8], matrix[12], matrix[1], matrix[5], matrix[9],
                  matrix[13], matrix[2], matrix[6], matrix[10], matrix[14], matrix[3], matrix[7],
                  matrix[11], matrix[15])
      .inverse();
}