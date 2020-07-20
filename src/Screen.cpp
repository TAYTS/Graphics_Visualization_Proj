#include "Screen.h"

using namespace std;

void Screen::addObjNode(string objKey, Drawable *objNode) { this->childNodes[objKey] = objNode; }

void Screen::drawScreen() {
  for (auto const &obj : this->childNodes) {
    obj.second->draw();
  }
};

Drawable *Screen::getObject(string objKey) {
  ObjMap::iterator obj = this->childNodes.find(objKey);
  if (obj != this->childNodes.end()) {
    return obj->second;
  } else {
    return NULL;
  }
}