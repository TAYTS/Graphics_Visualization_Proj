#ifndef BALL_H
#define BALL_H

#include <vector>

#include "vecmath.h"
#include "Maze.h"

class Ball : public Drawable {
public:
  Ball(Maze *maze, float radius);
  Ball(Maze *maze, float radius, float COR, Vector2f center,
       pair<Vector2f, Vector2f> movementLimit);

  // Render the model
  void draw();

  // Return the ball to the original position
  void resetPosition();

  // attributes
  float radius;
  float COR; // Coefficient of restituition
  Vector2f center, velocity;
  Maze *maze;
  pair<Vector2f, Vector2f> movementLimit;

private:
  Vector2f initCenter;

  // Compute the ball next position
  void computeNextPos();

  // Update the given velocity and position if it collide with any plane
  bool updateVelocityAndPositionIfCollide(Vector2f &velocity, Vector2f &center);

  // Get the matrix to convert the local coordinate system to world coordinate system
  Matrix4f getObjToWorldMatrix();

  // Get the matrix to convert the world coordinate system to local coordinate system
  Matrix4f getWorldToObjMatrix();
};

#endif
