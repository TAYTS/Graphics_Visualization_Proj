#include <iostream>

#include "Ball.h"
#include "Vector2f.h"
#include "Vector4f.h"
#include "Matrix3f.h"
#include "Screen.h"

using namespace std;

Ball::Ball(float radius) {
  this->radius = radius;
  this->center = Vector3f::ZERO;
  this->velocity = Vector3f::ZERO;
  this->transMat = Matrix4f::translation(0.0f, 0.0f, this->radius);
  this->setGravity(9.81f);
};

Ball::Ball(float radius, float COR, Vector3f center) : Ball(radius) {
  this->coefficientofRestituition = COR;
  this->center = center;
};

void Ball::pushTransMatrix(Matrix4f transMat) {
  this->transMat = this->transMat * transMat;
  this->transMat.print();
}

bool Ball::collide(Ray &r, Hit &h, float radius) { return false; }

void Ball::computeNextPos(float intervalTime) {
  Vector3f accDir = (this->globalToObjectTransMatrix * Vector4f(0.0f, -1.0f, 0.0f, 0.0f)).xyz();
  Vector2f acc = this->getGravity() * accDir.normalized().xy();
  Vector2f dir = intervalTime * this->velocity.xy() + 0.5 * intervalTime * intervalTime * acc;
  Ray ballPath(this->center.xy(), dir);
  Hit pathHit;
  do {
    bool collide = false;
    for (int i = 0; i < this->collidable.size(); i++) {
      collide = collide || this->collidable[i].collide(ballPath, pathHit, this->radius);
    }
    if (collide && pathHit.getT() < 1) {
      Vector2f dist = dir * pathHit.getT();
      this->center += Vector3f(dist, 0.0f);
      Vector2f determinant =
          (this->velocity * this->velocity + Vector3f(2 * acc * dist, 0.0f)).xy();
      int temp = abs(acc[0]) == 0.0f ? 1 : 0;
      float timeTaken1 = 0 - pow(determinant[temp], 0.5) - velocity[temp];
      timeTaken1 = timeTaken1 / acc[temp];
      float timeTaken2 = pow(determinant[temp], 0.5) - velocity[temp];
      timeTaken2 = timeTaken2 / acc[temp];
      float timeTaken = timeTaken1;
      if (timeTaken1 < 0.0f) {
        timeTaken = timeTaken2;
      } else if (timeTaken2 < timeTaken1 && timeTaken2 >= 0.0f) {
        timeTaken = timeTaken2;
      }
      this->velocity = this->velocity + Vector3f(acc * timeTaken, 0.0f);
      float speed = this->velocity.abs();
      this->velocity.negate();
      // this->velocity.print();
      this->velocity.normalize();
      this->velocity =
          2 * pathHit.getNormal() * Vector2f::dot(pathHit.getNormal(), this->velocity.xy()) -
          this->velocity;
      // this->velocity.print();
      this->velocity *= this->coefficientofRestituition * speed;
      // this->velocity.print();
      if (dist.abs() < 1e-21 && speed < 1e-21) {
        dir = Vector2f::ZERO;
        acc = Vector2f::ZERO;
        this->velocity = Vector3f::ZERO;
        break;
      }
      intervalTime -= timeTaken;
      pathHit.reset();
      dir = this->velocity.xy() * intervalTime + 0.5 * acc * intervalTime * intervalTime;
      Vector2f tempCenter = this->center.xy();
      ballPath.set(tempCenter, dir);
    } else {
      break;
    }
    // cout <<"interavalTime:"<< intervalTime<<"\n";
  } while (intervalTime > 0.0f);
  this->center += Vector3f(dir, 0.0f);
  this->velocity += Vector3f(acc * intervalTime, 0.0f);
}

void Ball::draw() {
  float mat[16];
  glGetFloatv(GL_MODELVIEW_MATRIX, mat);
  Matrix4f mazeTransMat =
      Matrix4f(mat[0], mat[4], mat[8], mat[12], mat[1], mat[5], mat[9], mat[13], mat[2], mat[6],
               mat[10], mat[14], mat[3], mat[7], mat[11], mat[15]);

  glPushMatrix();
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  // Set line color for render the gravity force line
  // change ball to different color
  GLfloat diffuseColor[4] = {1.0, 0.0, 0.0, 1.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
  Vector3f newCenter = (this->transMat * Vector4f(this->center, 1.0f)).xyz();
  glTranslated(newCenter.x(), newCenter.y(), newCenter.z());
  glutSolidSphere(this->radius, 12, 12);

  glPopAttrib();
  glPopMatrix();

  // compute vectors
  Vector3f centerVec = (mazeTransMat * this->transMat * Vector4f(this->center, 1.0f)).xyz();
  Vector3f mazeNormal = (mazeTransMat * Vector4f(-Vector3f::FORWARD, 0.0f)).xyz().normalized();
  Vector3f projGravityVec = this->GRAVITY - (Vector3f::dot(this->GRAVITY, mazeNormal)) * mazeNormal;
  Vector3f planeYAxis = (mazeTransMat * Vector4f(Vector3f::UP, 0.0f)).xyz().normalized();
  Vector3f planeXAxis = (mazeTransMat * Vector4f(Vector3f::RIGHT, 0.0f)).xyz().normalized();

  Vector3f gravityXAxis = Vector3f::dot(projGravityVec, planeXAxis);
  Vector3f gravityYAxis = Vector3f::dot(projGravityVec, planeYAxis);

  this->pushTransMatrix(Matrix4f::translation(gravityXAxis.x() / 60, gravityYAxis.x() / 60, 0));

  glPushMatrix();
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glLoadIdentity();
  glDisable(GL_LIGHTING);
  glLineWidth(5);

  glBegin(GL_LINES);
  glColor4f(1, 0, 0, 0);
  glVertex3fv(Vector3f::ZERO);
  glVertex3fv(mazeNormal.normalized() * 5);
  glEnd();

  glBegin(GL_LINES);
  glColor4f(0, 1, 0, 0);
  glVertex3fv(Vector3f::ZERO);
  glVertex3fv(projGravityVec.normalized() * 5);
  glEnd();

  glBegin(GL_LINES);
  glColor4f(0, 1, 1, 0);
  glVertex3fv(Vector3f::ZERO);
  glVertex3fv(this->GRAVITY.normalized() * 5);
  glEnd();

  glPopAttrib();
  glPopMatrix();
}