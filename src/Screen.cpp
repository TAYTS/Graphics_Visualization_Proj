#include "Screen.h"

using namespace std;

void Screen::addObjNode(Drawable *objNode) { this->childNodes.push_back(objNode); }

void Screen::drawScreen() {
  for (Drawable *obj : this->childNodes) {
    obj->draw();
  }
};