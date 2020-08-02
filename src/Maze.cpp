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
#include <algorithm>

#include "Maze.h"

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
    map<PlaneIndex, Plane> posFacingXPlaneMap, negFacingXPlaneMap, posFacingYPlaneMap,
        negFacingYPlaneMap;

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

        // Setup table for searching collide planes
        if (this->normals[n1 - 1].x() != 0.0f && this->normals[n2 - 1].x() != 0.0f &&
            this->normals[n3 - 1].x() != 0.0f) { // y-plane
          // Find the plane min & max position
          auto minmaxY = minmax(
              {this->vectices[v1 - 1].y(), this->vectices[v2 - 1].y(), this->vectices[v3 - 1].y()});

          // Create plane index struct to plane map indexing
          struct PlaneIndex yPlaneIndex {
            this->vectices[v1 - 1].x(), minmaxY.first, minmaxY.second
          };

          Plane yPlane = Plane(minmaxY.first, minmaxY.second, this->vectices[v1 - 1].x(),
                               this->normals[n1 - 1].xy());

          if (this->normals[n1 - 1].x() > 0.0f) { // Facing right/positive Y-plane
            posFacingYPlaneMap.insert({yPlaneIndex, yPlane});
          } else { // Facing left/negative Y-plane
            negFacingYPlaneMap.insert({yPlaneIndex, yPlane});
          }
        } else if (this->normals[n1 - 1].y() != 0.0f && this->normals[n2 - 1].y() != 0.0f &&
                   this->normals[n3 - 1].y() != 0.0f) { // x-plane
          // Find the plane min & max position
          auto minmaxX = minmax(
              {this->vectices[v1 - 1].x(), this->vectices[v2 - 1].x(), this->vectices[v3 - 1].x()});

          // Create plane index struct to plane map indexing
          struct PlaneIndex xPlaneIndex {
            this->vectices[v1 - 1].y(), minmaxX.first, minmaxX.second
          };

          Plane xPlane = Plane(minmaxX.first, minmaxX.second, this->vectices[v1 - 1].y(),
                               this->normals[n1 - 1].xy());

          if (this->normals[n1 - 1].y() > 0.0f) { // Facing top/positive X-plane
            posFacingXPlaneMap.insert({xPlaneIndex, xPlane});
          } else { // Facing bottom/positive X-plane
            negFacingXPlaneMap.insert({xPlaneIndex, xPlane});
          }
        }
      }
    }

    // Convert map to vector
    this->posFacingXPlane.reserve(posFacingXPlaneMap.size());
    for (auto planePair : posFacingXPlaneMap) {
      this->posFacingXPlane.push_back(planePair);
    }

    this->negFacingXPlane.reserve(negFacingXPlaneMap.size());
    for (auto planePair : negFacingXPlaneMap) {
      this->negFacingXPlane.push_back(planePair);
    }

    this->posFacingYPlane.reserve(posFacingYPlaneMap.size());
    for (auto planePair : posFacingYPlaneMap) {
      this->posFacingYPlane.push_back(planePair);
    }

    this->negFacingYPlane.reserve(negFacingYPlaneMap.size());
    for (auto planePair : negFacingYPlaneMap) {
      this->negFacingYPlane.push_back(planePair);
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
  // Update the Maze
  glBegin(GL_TRIANGLES);
  for (auto face : this->faces) {
    glNormal3d(this->normals.at(face.n1 - 1)[0], this->normals.at(face.n1 - 1)[1],
               this->normals.at(face.n1 - 1)[2]);
    glVertex3d(this->vectices.at(face.v1 - 1)[0], this->vectices.at(face.v1 - 1)[1],
               this->vectices.at(face.v1 - 1)[2]);
    glNormal3d(this->normals.at(face.n2 - 1)[0], this->normals.at(face.n2 - 1)[1],
               this->normals.at(face.n2 - 1)[2]);
    glVertex3d(this->vectices.at(face.v2 - 1)[0], this->vectices.at(face.v2 - 1)[1],
               this->vectices.at(face.v2 - 1)[2]);
    glNormal3d(this->normals.at(face.n3 - 1)[0], this->normals.at(face.n3 - 1)[1],
               this->normals.at(face.n3 - 1)[2]);
    glVertex3d(this->vectices.at(face.v3 - 1)[0], this->vectices.at(face.v3 - 1)[1],
               this->vectices.at(face.v3 - 1)[2]);
  }
  glEnd();
};

float Maze::getSizeX() { return this->maxX - this->minX; }
float Maze::getSizeY() { return this->maxZ - this->minZ; }
float Maze::getSizeZ() { return this->maxY - this->minY; }

Vector2f Maze::getMazeStartPos() { return Vector2f(0.0f, this->maxY); }

Vector2f Maze::getMazeEndPos() { return Vector2f(0.0f, this->minY); }

vector<pair<PlaneIndex, Plane> *> Maze::getPotentialCollidePlanePair(Ray &ray) {
  vector<pair<PlaneIndex, Plane> *> output;

  if (ray.getDirection().x() != 0) { // Check Y plane
    int yPlaneIdx = this->binarySearchPlane(
        ray, ray.getDirection().x() > 0 ? this->negFacingYPlane : this->posFacingYPlane, false);

    if (yPlaneIdx >= 0) {
      output.push_back(&(ray.getDirection().x() > 0 ? this->negFacingYPlane : this->posFacingYPlane)
                            .at(yPlaneIdx));
    }
  }

  if (ray.getDirection().y() != 0) { // Check X plane
    int xPlaneIdx = this->binarySearchPlane(
        ray, ray.getDirection().y() > 0 ? this->negFacingXPlane : this->posFacingXPlane, true);
    if (xPlaneIdx >= 0) {
      output.push_back(&(ray.getDirection().y() > 0 ? this->negFacingXPlane : this->posFacingXPlane)
                            .at(xPlaneIdx));
    }
  }
  return output;
}

int Maze::binarySearchPlane(Ray &ray, vector<pair<PlaneIndex, Plane>> &planeTable,
                            bool xPlane) const {
  int lo = 0, hi = planeTable.size() - 1, mid = 0;
  float startX = ray.getOrigin().x();
  float startY = ray.getOrigin().y();
  float endX = ray.getEndpoint().x();
  float endY = ray.getEndpoint().y();

  while (lo <= hi) {
    mid = lo + ((hi - lo) / 2);

    pair<PlaneIndex, Plane> planePair = planeTable.at(mid);

    if (xPlane) {
      if (ray.getDirection().y() > 0) { // ray pointing upward, plane pointing downward
        if (planePair.first.planePos >= startY && planePair.first.planePos <= endY) {
          // X plane is in between the start and end of the ray
          if ((startX >= planePair.first.minPos && startX <= planePair.first.maxPos) ||
              (endX >= planePair.first.minPos && endX <= planePair.first.maxPos)) {
            // ray is in between the plane
            return mid;
          } else if (startX > planePair.first.minPos &&
                     startX > planePair.first.maxPos) { // X plane is on the left side of the ray
            lo = mid + 1;
          } else { // X plane is on the right side of the ray
            hi = mid - 1;
          }
        } else if (planePair.first.planePos < startY) { // X plane is below the start of the ray
          lo = mid + 1;
        } else { // X plane above the end of the ray
          hi = mid - 1;
        }
      } else if (ray.getDirection().y() < 0) { // ray pointing downward, plane pointing upward
        if (planePair.first.planePos <= startY && planePair.first.planePos >= endY) {
          // X plane is in between the start and end of the ray
          if ((startX >= planePair.first.minPos && startX <= planePair.first.maxPos) ||
              (endX >= planePair.first.minPos && endX <= planePair.first.maxPos)) {
            // ray is in between the plane
            return mid;
          } else if (startX > planePair.first.minPos &&
                     startX > planePair.first.maxPos) { // X plane is on the left side of the ray
            lo = mid + 1;
          } else { // X plane is on the right side of the ray
            hi = mid - 1;
          }
        } else if (planePair.first.planePos > startY) { // X plane is above the start of the ray
          hi = mid - 1;
        } else { // X plane below the end of the ray
          lo = mid + 1;
        }
      } else { // ray is either parallel or a point
        return -1;
      }
    } else {                            // Y plane
      if (ray.getDirection().x() > 0) { // ray pointing right, plane pointing left
        if (planePair.first.planePos >= startX && planePair.first.planePos <= endX) {
          // Y plane is in between the start and end of the ray
          if ((startY >= planePair.first.minPos && startY <= planePair.first.maxPos) ||
              (endY >= planePair.first.minPos && endY <= planePair.first.maxPos)) {
            // ray is in between the plane
            return mid;
          } else if (startY > planePair.first.minPos &&
                     startY > planePair.first.maxPos) { // Y plane is below the ray
            lo = mid + 1;
          } else {
            hi = mid - 1;
          }
        } else if (planePair.first.planePos < startX) { // Y plane is on the left side of the ray
          lo = mid + 1;
        } else { // Y plane is on the right side of the ray
          hi = mid - 1;
        }
      } else if (ray.getDirection().x() < 0) { // ray pointing left, plane pointing right
        if (planePair.first.planePos <= startX && planePair.first.planePos >= endX) {
          // Y plane is in between the start and end of the ray
          if ((startY >= planePair.first.minPos && startY <= planePair.first.maxPos) ||
              (endY >= planePair.first.minPos && endY <= planePair.first.maxPos)) {
            // ray is in between the plane
            return mid;
          } else if (startY > planePair.first.minPos &&
                     startY > planePair.first.maxPos) { // Y plane is below the ray
            lo = mid + 1;
          } else {
            hi = mid - 1;
          }
        } else if (planePair.first.planePos > startX) { // Y plane is on the right side of the ray
          hi = mid - 1;
        } else { // Y plane is on the left side of the ray
          lo = mid + 1;
        }
      } else { // ray is either parallel or a point
        return -1;
      }
    }
  }

  return -1;
}