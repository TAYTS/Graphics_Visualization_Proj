#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#elif defined _WIN32 || defined _WIN64
#include <glut.h>
#endif

#include <iostream>
#include "vecmath.h"
#include "Maze.h"

using namespace std;

////////////////////////////////////
//        Global Variables        //
////////////////////////////////////
#define _ZNEAR 1.0
#define _ZFAR 100.0
#define _ROTSCALE 0.5
#define MAX_BUFFER_SIZE 1024

// Current field of view
const double FOV = 60;

// Mouse interaction
GLint mouseModifiers = 0;
GLint mouseButton = 0;
int prevMouseX, prevMouseY;

// Material color table
GLfloat colorTable[4][4] = {
    {0.5, 0.5, 0.9, 1.0}, {0.9, 0.5, 0.5, 1.0}, {0.5, 0.9, 0.3, 1.0}, {0.3, 0.8, 0.9, 1.0}};

// Current material color and color ID
GLfloat diffuseColor[4];
int colorID = 0;

Maze maze;

////////////////////////////////////
//      Modification Functions    //
////////////////////////////////////
void SetDiffuseColor(int colorID) {
  // Set the diffueColor using the colorTable and current colorID
  for (int i = 0; i < sizeof(diffuseColor) / sizeof(diffuseColor[0]); i++) {
    diffuseColor[i] = colorTable[colorID][i];
  }

  // Set the object diffuse color
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);
}

void RotateModel(double angle, Vector3f axis) {
  double mat[16];

  glGetDoublev(GL_MODELVIEW_MATRIX, mat);
  glLoadIdentity();
  glTranslated(mat[12], mat[13], mat[14]);
  glRotated(angle, axis[0], axis[1], axis[2]);
  glTranslated(-mat[12], -mat[13], -mat[14]);

  glMultMatrixd(mat);
}

void ScaleModel(double scale) {
  double mat[16];

  glGetDoublev(GL_MODELVIEW_MATRIX, mat);

  glLoadIdentity();
  glTranslated(mat[12], mat[13], mat[14]);
  glScaled(scale, scale, scale);
  glTranslated(-mat[12], -mat[13], -mat[14]);

  glMultMatrixd(mat);
}

////////////////////////////////////
//       Rendering Functions      //
////////////////////////////////////
void resetView() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -10.0f);
}

void resetProjectionView() {
  int w = glutGet(GLUT_WINDOW_WIDTH);
  int h = glutGet(GLUT_WINDOW_HEIGHT);

  GLdouble aspectRatio = 1.0f * w / (h == 0 ? 1 : h);
  // Update projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOV, aspectRatio, _ZNEAR, _ZFAR);

  // Reset matrix mode
  glMatrixMode(GL_MODELVIEW);
}

void reshape(int w, int h) {
  glViewport(0, 0, w * 2, h * 2);
  resetProjectionView();
}

void initRendering() {
  GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
  GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat shininess[] = {50.0};
  SetDiffuseColor(colorID);

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

  // 2. set the projection and modelview
  resetView();
  resetProjectionView();

  // 3. Material

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);

  // 4. Lighting
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_POSITION, position);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glPopMatrix();
}

////////////////////////////////////
//         Event Functions        //
////////////////////////////////////
void keyboard(unsigned char key, int x, int y) {
  const float scale_out = 0.95;
  const float scale_in = 1.05;

  switch (key) {
  case 27: // Escape key
    exit(0);
    break;

  case 's':
    // Make the model smaller
    ScaleModel(scale_out);
    break;

  case 'b':
    // Make the model bigger
    ScaleModel(scale_in);
    break;

  case 't':
    // Reset the model pose
    resetView();
    break;

  default:
    cout << "Unhandled key press " << key << "." << endl;
  }

  // this will refresh the screen so that the user sees the color change
  glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
  // flip it first
  y = glutGet(GLUT_WINDOW_HEIGHT) - 1 - y;

  // since users might just use left/middle button to
  // close the pop-up menu, we cannot simply use XOR
  // here to compute mouseButton, we have to use two
  // cases here:
  if (state == GLUT_DOWN)
    mouseButton = mouseButton | (1 << (button));
  else
    mouseButton = mouseButton & (~(1 << (button)));

  mouseModifiers = glutGetModifiers();

  prevMouseX = x;
  prevMouseY = y;
}

void motion(int x, int y) {
  double mat[16];
  double tx, ty, nx, ny, scale, angle;
  int dx, dy;

  y = glutGet(GLUT_WINDOW_HEIGHT) - 1 - y;
  dx = x - prevMouseX;
  dy = y - prevMouseY;

  if (dx == 0 && dy == 0)
    return;

  prevMouseX = x;
  prevMouseY = y;

  glMatrixMode(GL_MODELVIEW);

  switch (mouseButton) {
    ////////////////////////////////////////
    // LEFT BUTTON

  case 0x1:

    // Scale the model
    if (mouseModifiers == GLUT_ACTIVE_SHIFT) {
      if (dy > 0)
        scale = 1 + 0.001 * sqrt(dx * dx + dy * dy);
      else
        scale = 1 - 0.001 * sqrt(dx * dx + dy * dy);

      ScaleModel(scale);
    }

    // Rotate the model
    else {
      // Rotation
      nx = -dy;
      ny = dx;
      scale = sqrt(nx * nx + ny * ny);

      // We use "ArcBall Rotation" to compute the rotation axis and angle based
      // on the mouse motion
      nx = nx / scale;
      ny = ny / scale;
      angle = scale * _ROTSCALE * FOV / 90.0;

      RotateModel(angle, Vector3f(nx, ny, 0));
    }

    glutPostRedisplay();
    break;

  // MIDDLE BUTTON
  case 0x2:
    break;

  // RIGHT BUTTON
  case 0x4:
    break;
  }
}

void drawScene(void) {
  // Clear the rendering window
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  maze.renderModel();

  // Dump the image to the screen.
  glutSwapBuffers();
}

// Setup the initial window size, position and title
void windowSetup(int winWidth, int winHeight, int winPosX, int winPosY, string winName) {
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(winWidth, winHeight);
  glutInitWindowPosition(winPosX, winPosY);
  glutCreateWindow(winName.data());
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);

  // Load object
  maze.loadObj("../data/maze_15x15.obj");

  // Setup the display window
  int winWidth, windHeight, winPosX, winPosY;
  winWidth = windHeight = 800;
  winPosX = winPosY = 100;
  string winName = "Maze Game";
  windowSetup(winWidth, windHeight, winPosX, winPosY, winName);

  // Initialize OpenGL parameters.
  initRendering();

  // Post Event Handlers
  glutReshapeFunc(reshape);
  glutDisplayFunc(drawScene);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  // Rendering loop
  glutMainLoop();

  return 0; // This line is never reached.
}