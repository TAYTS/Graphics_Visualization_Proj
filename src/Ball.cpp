#include <iostream>
#include <algorithm>

#include "Ball.h"
#include "Screen.h"
#include "Ray.h"

using namespace std;

////////////////////////////////////
//           Constructor          //
////////////////////////////////////
Ball::Ball(Maze *maze, float radius) {
  this->maze = maze;
  this->radius = radius;
  this->COR = 1.0f;
  this->center = Vector2f::ZERO;
  this->initCenter = Vector2f::ZERO;
  this->velocity = Vector2f::ZERO;
  this->movementLimit = pair<Vector2f, Vector2f>(Vector2f::ZERO, Vector2f::ZERO);
};

Ball::Ball(Maze *maze, float radius, float COR, Vector2f center,
           pair<Vector2f, Vector2f> movementLimit)
    : Ball(maze, radius) {
  this->COR = COR;
  this->center = center;
  this->initCenter = center;
  this->movementLimit = movementLimit;
};

////////////////////////////////////
//         Public Methods         //
////////////////////////////////////
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

void Ball::resetPosition() {
  this->center = this->initCenter;
  this->velocity = Vector2f::ZERO;
}

vector<Vector2f> getNormalDirectionFromVelocity(Vector2f *velocity) {
  vector<Vector2f> directions;

  if (velocity->x() > 0.0000001f) {
    directions.push_back(Vector2f(1.0f, 0.0f));
  } else if (velocity->x() < -0.0000001f) {
    directions.push_back(Vector2f(-1.0f, 0.0f));
  }

  if (velocity->y() > 0.0000001f) {
    directions.push_back(Vector2f(0.0f, 1.0f));
  } else if (velocity->y() < -0.0000001f) {
    directions.push_back(Vector2f(0.0f, -1.0f));
  }

  return directions;
}

////////////////////////////////////
//        Private Methods         //
////////////////////////////////////
void Ball::computeNextPos() {
  Matrix4f objToWorldMatrix = this->getObjToWorldMatrix();
  Matrix4f worldToObjMatrix = this->getWorldToObjMatrix();

  bool collide = false;
  float timestep = REFRESH_RATE / 520.0f;

  Vector3f gravityAccel = Vector3f(0.0, 0.0, -GRAVITY); // gravity force point into the screen
  // Maze normal in world coordinate system
  Vector3f mazeNormal = (objToWorldMatrix * Vector4f(-Vector3f::FORWARD, 0.0f)).xyz().normalized();

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
      newVelocity -= Vector2f(newVelocity.x() * 0.08f, 0.0f);
    }
    if (newVelocity.y() != 0) {
      newVelocity -= Vector2f(0.0f, newVelocity.y() * 0.08f);
    }
  }

  // formula: delta_distance = 0.5 * (v1 + v0) * time_delta
  Vector2f totalDisplacement = 0.5 * (this->velocity + newVelocity) * timestep;
  Vector2f newCenter = this->center + totalDisplacement;

  // Check if the new center exit the maze boundary
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

  // Check the magnitude of the displacement, if the displacement too large use distance step
  if (abs(totalDisplacement.x()) >= this->radius || abs(totalDisplacement.y()) >= this->radius) {
    float xStep = 0.0f, yStep = 0.0f, stepSize = 0.02f;
    Vector2f tempCenter = this->center;

    if (abs(totalDisplacement.x()) >= this->radius) {
      if (totalDisplacement.x() > 0.0000001f) {
        xStep = stepSize;
      } else if (totalDisplacement.x() < -0.0000001f) {
        xStep = -stepSize;
      }
    } else {
      tempCenter.x() += totalDisplacement.x();
    }

    if (abs(totalDisplacement.y()) >= this->radius) {
      if (totalDisplacement.y() > 0.0000001f) {
        yStep = stepSize;
      } else if (totalDisplacement.y() < -0.0000001f) {
        yStep = -stepSize;
      }
    } else {
      tempCenter.y() += totalDisplacement.y();
    }

    tempCenter += Vector2f(xStep, yStep);

    bool collide = false;

    while ((xStep == 0.0f || (xStep > 0.0f && tempCenter.x() < newCenter.x()) ||
            (xStep < 0.0f && tempCenter.x() > newCenter.x())) &&
           (yStep == 0.0f || (yStep > 0.0f && tempCenter.y() < newCenter.y()) ||
            (yStep < 0.0f && tempCenter.y() > newCenter.y()))) {
      collide = this->updateVelocityAndPositionIfCollide(newVelocity, tempCenter);

      if (collide) {
        break;
      }

      if ((xStep > 0.0f && tempCenter.x() < newCenter.x()) ||
          (xStep < 0.0f && tempCenter.x() > newCenter.x())) {
        tempCenter.x() += xStep;
      }

      if ((yStep > 0.0f && tempCenter.y() < newCenter.y()) ||
          (yStep < 0.0f && tempCenter.y() > newCenter.y())) {
        tempCenter.y() += yStep;
      }
    }

    // Replace the newCenter with the updated center
    newCenter = tempCenter;
  } else {
    this->updateVelocityAndPositionIfCollide(newVelocity, newCenter);
  }

  this->velocity = newVelocity;
  this->center = newCenter;
}

bool Ball::updateVelocityAndPositionIfCollide(Vector2f &velocity, Vector2f &center) {
  bool collide = false;
  // Get the directions in the major axis based on the velocity vector
  vector<Vector2f> dirs = {
      Vector2f(1, 0), Vector2f(-1, 0), Vector2f(0, 1),  Vector2f(0, -1),
      Vector2f(1, 1), Vector2f(1, -1), Vector2f(-1, 1), Vector2f(-1, -1),
  };

  for (auto dir : dirs) {
    Ray ballDirection = Ray(center, dir, this->radius);

    vector<pair<PlaneIndex, Plane> *> potentialCollidePlane =
        this->maze->getPotentialCollidePlanePair(ballDirection);

    if (potentialCollidePlane.size() > 0) {
      for (auto planePair : potentialCollidePlane) {
        Vector2f difference = Vector2f::ZERO;
        collide = planePair->second.collide(&ballDirection, difference);

        if (collide) {
          if (planePair->second.normal.x() != 0.0f) { // Y plane
            float penetration = this->radius - abs(difference.x());
            penetration = floor(penetration * 1000000.0f) / 1000000.0f;

            // flip the vector direction
            if ((planePair->second.normal.x() > 0 && velocity.x() < 0) ||
                (planePair->second.normal.x() < 0 && velocity.x() > 0)) {
              velocity.x() *= (-1.0f * this->COR);
            }
            if (planePair->second.normal.x() > 0.0f) {
              center.x() += penetration;
            } else {
              center.x() -= penetration;
            }
          } else { // X plane
            float penetration = this->radius - abs(difference.y());
            penetration = floor(penetration * 1000000.0f) / 1000000.0f;

            if ((planePair->second.normal.y() > 0 && velocity.y() < 0) ||
                (planePair->second.normal.y() < 0 && velocity.y() > 0)) {
              velocity.y() *= (-1.0f * this->COR);
            }

            if (planePair->second.normal.y() > 0.0f) {
              center.y() += penetration;
            } else {
              center.y() -= penetration;
            }
          }
        }
      }
    }
    if (collide) {
      return collide;
    }
  }

  return collide;
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