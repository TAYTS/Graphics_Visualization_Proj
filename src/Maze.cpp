#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#elif defined _WIN32 || defined _WIN64
#include <glut.h>
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include "Maze.h"

#define TEX_SCALE_FACTOR 5 //used to scale number of texture units per unit maze length for width and height

using namespace std;

Maze::Maze() {}
Maze::Maze(string objFilePath) { this->loadObj(objFilePath); }

void Maze::loadObj(string objFilePath) {
  if (objFilePath.find(".obj") != string::npos) {
    ifstream fin(objFilePath, ios::in);
    if (!fin) {
      cerr << "Invalid file" << endl;
      exit(1);
    }

    this->minX = numeric_limits<float>::max();
    this->minY = numeric_limits<float>::max();
    this->minZ = numeric_limits<float>::max();
    this->maxX = numeric_limits<float>::min();
    this->maxY = numeric_limits<float>::min();
    this->maxZ = numeric_limits<float>::min();

    string lineBuffer;
    while (getline(fin, lineBuffer)) {
      if (lineBuffer.substr(0, 2) == "v ") { // parse vertices data
        istringstream v(lineBuffer.substr(2));
        float x, y, z;
        v >> x;
        v >> y;
        v >> z;

        if (x < this->minX)
          this->minX = x;
        if (x > this->maxX)
          this->maxX = x;
        if (y < this->minY)
          this->minY = y;
        if (y > this->maxY)
          this->maxY = y;
        if (z < this->minZ)
          this->minZ = z;
        if (z > this->maxZ)
          this->maxZ = z;

        this->vectices.push_back(Vector3f(x, y, z));
      } else if (lineBuffer.substr(0, 2) == "vn") { // parse normals data
        istringstream v(lineBuffer.substr(2));
        float x, y, z;
        v >> x;
        v >> y;
        v >> z;
        this->normals.push_back(Vector3f(x, y, z));
      } else if (lineBuffer.substr(0, 2) == "f ") { // parse faces data
        unsigned int v1, v2, v3;
        unsigned int n1, n2, n3;
        const char *face = lineBuffer.c_str();
        int match = sscanf(face, "f %i//%i %i//%i %i//%i", &v1, &n1, &v2, &n2, &v3, &n3);
        if (match != 6) {
          cerr << "Failed to parse OBJ file" << endl;
          exit(1);
        }

        struct Face f {
          v1, v2, v3, n1, n2, n3
        };
        this->faces.push_back(f);
      }
    }

    // Close the file
    fin.close();
    cout << "Done loading obj file" << endl;
    return;
  } else {
    cerr << "Invalid file" << endl;
    exit(1);
  }

}

void Maze::draw() {
  // enable texturing
  glEnable(GL_TEXTURE_2D);
  
  for (auto face : this->faces) {
    if ((this->normals.at(face.n1 - 1)[2] + this->normals.at(face.n2 - 1)[2] +
        this->normals.at(face.n3 - 1)[2])/3 > 0.5f) {
        // check if face is facing +z, towards player

        if (this->vectices.at(face.v1 - 1)[2] <= 0.2f &&
            this->vectices.at(face.v2 - 1)[2] <= 0.2f &&
            this->vectices.at(face.v3 - 1)[2] <= 0.2f) {
          // applies floor texture to faces that are the floor
          glBindTexture(GL_TEXTURE_2D, floorTex);
          glBegin(GL_TRIANGLES);
          glTexCoord2d(0 + 0.5 * (this->vectices.at(face.v1 - 1)[0] - minX) * TEX_SCALE_FACTOR /
                               (maxX - minX),
                       0 + 0.5 * (this->vectices.at(face.v1 - 1)[1] - minY) * TEX_SCALE_FACTOR /
                               (maxY - minY));
          glNormal3d(this->normals.at(face.n1 - 1)[0], this->normals.at(face.n1 - 1)[1],
                     this->normals.at(face.n1 - 1)[2]);
          glVertex3d(this->vectices.at(face.v1 - 1)[0], this->vectices.at(face.v1 - 1)[1],
                     this->vectices.at(face.v1 - 1)[2]);
          glTexCoord2d(0 + 0.5 * (this->vectices.at(face.v2 - 1)[0] - minX) * TEX_SCALE_FACTOR /
                               (maxX - minX),
                       0 + 0.5 * (this->vectices.at(face.v2 - 1)[1] - minY) * TEX_SCALE_FACTOR /
                               (maxY - minY));
          glNormal3d(this->normals.at(face.n2 - 1)[0], this->normals.at(face.n2 - 1)[1],
                     this->normals.at(face.n2 - 1)[2]);
          glVertex3d(this->vectices.at(face.v2 - 1)[0], this->vectices.at(face.v2 - 1)[1],
                     this->vectices.at(face.v2 - 1)[2]);
          glTexCoord2d(0 + 0.5 * (this->vectices.at(face.v3 - 1)[0] - minX) * TEX_SCALE_FACTOR /
                               (maxX - minX),
                       0 + 0.5 * (this->vectices.at(face.v3 - 1)[1] - minY) * TEX_SCALE_FACTOR /
                               (maxY - minY));
          glNormal3d(this->normals.at(face.n3 - 1)[0], this->normals.at(face.n3 - 1)[1],
                     this->normals.at(face.n3 - 1)[2]);
          glVertex3d(this->vectices.at(face.v3 - 1)[0], this->vectices.at(face.v3 - 1)[1],
                     this->vectices.at(face.v3 - 1)[2]);
          glEnd();
        } else {
          // applies texture to faces that are on the top of the maze
          glBindTexture(GL_TEXTURE_2D, topTex);
          glBegin(GL_TRIANGLES);
          glTexCoord2d(0.5 + 0.5 * (this->vectices.at(face.v1 - 1)[0] - minX) * TEX_SCALE_FACTOR /
                                 (maxX - minX),
                       0.5 + 0.5 * (this->vectices.at(face.v1 - 1)[1] - minY) * TEX_SCALE_FACTOR /
                                 (maxY - minY));
          glNormal3d(this->normals.at(face.n1 - 1)[0], this->normals.at(face.n1 - 1)[1],
                     this->normals.at(face.n1 - 1)[2]);
          glVertex3d(this->vectices.at(face.v1 - 1)[0], this->vectices.at(face.v1 - 1)[1],
                     this->vectices.at(face.v1 - 1)[2]);
          glTexCoord2d(0.5 + 0.5 * (this->vectices.at(face.v2 - 1)[0] - minX) * TEX_SCALE_FACTOR /
                                 (maxX - minX),
                       0.5 + 0.5 * (this->vectices.at(face.v2 - 1)[1] - minY) * TEX_SCALE_FACTOR /
                                 (maxY - minY));
          glNormal3d(this->normals.at(face.n2 - 1)[0], this->normals.at(face.n2 - 1)[1],
                     this->normals.at(face.n2 - 1)[2]);
          glVertex3d(this->vectices.at(face.v2 - 1)[0], this->vectices.at(face.v2 - 1)[1],
                     this->vectices.at(face.v2 - 1)[2]);
          glTexCoord2d(0.5 + 0.5 * (this->vectices.at(face.v3 - 1)[0] - minX) * TEX_SCALE_FACTOR /
                                 (maxX - minX),
                       0.5 + 0.5 * (this->vectices.at(face.v3 - 1)[1] - minY) * TEX_SCALE_FACTOR /
                                 (maxY - minY));
          glNormal3d(this->normals.at(face.n3 - 1)[0], this->normals.at(face.n3 - 1)[1],
                     this->normals.at(face.n3 - 1)[2]);
          glVertex3d(this->vectices.at(face.v3 - 1)[0], this->vectices.at(face.v3 - 1)[1],
                     this->vectices.at(face.v3 - 1)[2]);
          glEnd();
        }
    } else if(fabs(this->normals.at(face.n1 - 1)[1] + this->normals.at(face.n2 - 1)[1] +
        this->normals.at(face.n3 - 1)[1]) >0.5f){
      // checks that face is facing +y or -y direction and applies wall texture, mapped to the x coordinates

      glBindTexture(GL_TEXTURE_2D, wallTex);
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0 + 0.5 * (this->vectices.at(face.v1 - 1)[0] - minX) / (maxZ),
                   0.5 + 0.5 * (this->vectices.at(face.v1 - 1)[2] ) / (maxZ ));
      glNormal3d(this->normals.at(face.n1 - 1)[0], this->normals.at(face.n1 - 1)[1],
                 this->normals.at(face.n1 - 1)[2]);
      glVertex3d(this->vectices.at(face.v1 - 1)[0], this->vectices.at(face.v1 - 1)[1],
                 this->vectices.at(face.v1 - 1)[2]);
      glTexCoord2d(0 + 0.5 * (this->vectices.at(face.v2 - 1)[0] - minX) / (maxZ),
                   0.5 + 0.5 * (this->vectices.at(face.v2 - 1)[2] ) / (maxZ));
      glNormal3d(this->normals.at(face.n2 - 1)[0], this->normals.at(face.n2 - 1)[1],
                 this->normals.at(face.n2 - 1)[2]);
      glVertex3d(this->vectices.at(face.v2 - 1)[0], this->vectices.at(face.v2 - 1)[1],
                 this->vectices.at(face.v2 - 1)[2]);
      glTexCoord2d(0 + 0.5 * (this->vectices.at(face.v3 - 1)[0] - minX) / (maxZ),
                   0.5 + 0.5 * (this->vectices.at(face.v3 - 1)[2] ) / (maxZ));
      glNormal3d(this->normals.at(face.n3 - 1)[0], this->normals.at(face.n3 - 1)[1],
                 this->normals.at(face.n3 - 1)[2]);
      glVertex3d(this->vectices.at(face.v3 - 1)[0], this->vectices.at(face.v3 - 1)[1],
                 this->vectices.at(face.v3 - 1)[2]);
      glEnd();
    } else {
      // else the face is facing +y or -y and applies wall texture, mapped to the y coordinates
      glBindTexture(GL_TEXTURE_2D, wallTex);
      glBegin(GL_TRIANGLES);
      glTexCoord2d(0 + 0.5 * (this->vectices.at(face.v1 - 1)[1] - minY) / (maxZ),
                   0.5 + 0.5 * (this->vectices.at(face.v1 - 1)[2]) / (maxZ));
      glNormal3d(this->normals.at(face.n1 - 1)[0], this->normals.at(face.n1 - 1)[1],
                 this->normals.at(face.n1 - 1)[2]);
      glVertex3d(this->vectices.at(face.v1 - 1)[0], this->vectices.at(face.v1 - 1)[1],
                 this->vectices.at(face.v1 - 1)[2]);
      glTexCoord2d(0 + 0.5 * (this->vectices.at(face.v2 - 1)[1] - minY) / (maxZ),
                   0.5 + 0.5 * (this->vectices.at(face.v2 - 1)[2]) / (maxZ));
      glNormal3d(this->normals.at(face.n2 - 1)[0], this->normals.at(face.n2 - 1)[1],
                 this->normals.at(face.n2 - 1)[2]);
      glVertex3d(this->vectices.at(face.v2 - 1)[0], this->vectices.at(face.v2 - 1)[1],
                 this->vectices.at(face.v2 - 1)[2]);
      glTexCoord2d(0 + 0.5 * (this->vectices.at(face.v3 - 1)[1] - minY) / (maxZ),
                   0.5 + 0.5 * (this->vectices.at(face.v3 - 1)[2]) / (maxZ));
      glNormal3d(this->normals.at(face.n3 - 1)[0], this->normals.at(face.n3 - 1)[1],
                 this->normals.at(face.n3 - 1)[2]);
      glVertex3d(this->vectices.at(face.v3 - 1)[0], this->vectices.at(face.v3 - 1)[1],
                 this->vectices.at(face.v3 - 1)[2]);
      glEnd();
    }
  }
  

  glDisable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
};

float Maze::getSizeX() { return this->maxX - this->minX; }
float Maze::getSizeY() { return this->maxZ - this->minZ; }
float Maze::getSizeZ() { return this->maxY - this->minY; }

Vector2f Maze::getMazeStartPos() { return Vector2f(0.0f, this->maxY); }

Vector2f Maze::getMazeEndPos() { return Vector2f(0.0f, this->minY); }