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
};

Ball::Ball(float radius, Vector3f center) : Ball(radius) { this->center = center; };

Ball::Ball(float radius, Vector3f center, Vector3f velocity) : Ball(radius, center) {
  this->velocity = velocity;
}

void Ball::pushTransMatrix(Matrix4f transMat) {
  this->transMat = this->transMat * transMat;
  this->transMat.print();
}

void Ball::updateNextPosition() {}

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