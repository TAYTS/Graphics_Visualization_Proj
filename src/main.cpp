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
#include "BallDropTest.h"

using namespace std;

int main(int argc, char **argv) {
  BallDropTest(argc,argv);
	return 0;
}
