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


using namespace std;

#define _ZNEAR 0.1
#define _ZFAR 100.0
#define _ROTSCALE 0.5

#define MAX_BUFFER_SIZE 1024

#define refreshTime 40

// Window size and position
int winW = 800;
int winH = 800;
int winPosX = 60;
int winPosY = 60;

// Current field of view
double currFovy = 60;

GLfloat colour[4] = {0.3, 0.3, 0.9, 1.0};

Plane basePlane(Vector3f(0, 1.0f, 0), -0.8f);
Plane topPlane(Vector3f(0, -1.0f, 0), 0.8f);
Ball testBall(Vector2f(0, 1.23f), 0.2f, 0.95f);

void resetView() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -3.0f); // move the object -5.0 along eye-Z away from eye
}

void resetProj() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(currFovy, winW / ((double)winH), _ZNEAR, _ZFAR);
  glMatrixMode(GL_MODELVIEW);
}

void initRendering() {
  GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
  GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat shininess[] = {100.0};
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colour);

  GLfloat position[] = {0.0f, 0.0f, 1.0f, 0.0f};

  ////////////////////////////////////////////////////////////////////
  // 1. various status

  glEnable(GL_DEPTH_TEST);
  glClearDepth(1.0f);
  glDepthFunc(GL_LESS);

  // Anti-aliasing
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_POINT_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);

  // Auto-Normalization
  glEnable(GL_NORMALIZE);

  // Cull the back face (speedup and transparency)
  glCullFace(GL_BACK);

  ////////////////////////////////////////////////////////////////////
  // 2. set the projection and modelview

  resetView();

  ////////////////////////////////////////////////////////////////////
  // 3. Material

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colour);

  ////////////////////////////////////////////////////////////////////
  // 4. Lighting

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
  // glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuseColor);
  // glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  glEnable(GL_LIGHTING); // Enable lighting calculations
  glEnable(GL_LIGHT0);

  glPopMatrix();
}

void reshape(int w, int h) {
  winW = w;
  winH = h;

  glViewport(0, 0, winW, winH);

  resetProj();
}


void renderModel() {
	
	/*
	glPushMatrix();

	//testBall.getCenter().print();
    glMultMatrixf(Matrix4f::translation(Vector3f(testBall.getCenter(),0.0f)));
    glutSolidSphere(testBall.getRadius(), 12, 12);
    glPopMatrix();*/

	testBall.draw();
}

void drawScene(void) {
  // Clear the rendering window
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  renderModel();

  // Dump the image to the screen.
  glutSwapBuffers();
}

void physicsCycle(int t) {
  //cout << "frame\n\n";
  testBall.computeNextPos(0.02f);
  glutPostRedisplay();
  glutTimerFunc(refreshTime, physicsCycle, 0);
}

void BallDropTest(int &argc, char **argv) {

  glutInit(&argc, argv);

  testBall.addCollidable(basePlane);
  testBall.addCollidable(topPlane);
  testBall.setGravity(90.0f);

  // We're going to animate it, so double buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  // Initial parameters for window position and size
  glutInitWindowPosition(winPosX, winPosY);
  glutInitWindowSize(winW, winH);
  glutCreateWindow("Ball drop test");

  // Initialize OpenGL parameters.
  initRendering();

  // Post Event Handlers
  glutReshapeFunc(reshape);
  glutDisplayFunc(drawScene);

  glutTimerFunc(refreshTime, physicsCycle, 0);

  glutMainLoop();
}