#ifndef BALLDROPTEST_H
#define BALLDROPTEST_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#elif defined _WIN32 || defined _WIN64
#include <glut.h>
#endif

#include <vector>
#include "Physics.h"
#include "Plane.h"
#include "Ball.h"

void BallDropTest(int &argc, char **argv);

#endif // BALLDROPTEST_H