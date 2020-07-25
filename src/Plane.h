#ifndef PLANE_H
#define PLANE_H

#include "Physics.h"
#include "Ray.h"
#include "vecmath.h"
#include <cmath>
#include <iostream>
using namespace std;


/// TODO: Implement Plane representing an infinite plane
class Plane:public Physics
{
public:
	Plane(){}

	Plane( const Vector3f& normal , float d){
        this->normal = normal.normalized();
        this->d = d;
	}

	~Plane(){}


    // TODO
	virtual bool collide(Ray &r, Hit &h, float radius) {
		//cout << "test Plane \n";
		Vector2f vecO = r.getOrigin();
		Vector2f vecD = r.getDirection();
		//vecO.print();
		//vecD.print();
		//Vector3f dist = this->d;
		float dirDot = Vector2f::dot(this->normal.xy(), vecD);
        //float originDot = Vector2f::dot(this->normal.xy(), vecO);
        //float offset = this->d + radius * this->normal.y();
		float t1 = ((this->d+radius*this->normal.y())*this->normal.y() - Vector2f::dot(this->normal.xy(), vecO)) / dirDot;
        //cout <<"offset: "<<offset<<" dir: " << dirDot <<" originDot"<<originDot<<" t1:" << t1 << "\n";
		//this->normal.print();
		//vecO.print();
		//vecD.print();
		//cout << t1 << "\n";
        //Vector2f hitPoint = vecO + t1 * vecD;
        //cout << "hitPoint: ";
        //hitPoint.print();
		if (t1 <0.0f||dirDot>=0.0f) {
			return false;
		}
		if (t1 < h.getT()) {
			h.set(t1, this->normal.xy());
			return true;
		}
		return false;
	}

protected:
    Vector3f normal;
    float d;
    
};
#endif //PLANE_H
		

