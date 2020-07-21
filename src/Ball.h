#ifndef BALL_H
#define BALL_H

#include "Physics.h"
#include "vecmath.h"
#include <vector>
#include <cmath>
#include<float.h>
#include "Ray.h"

#include <iostream>
using namespace std;
 
class Ball: public Physics
{
public:
    Ball(){ 
		//unit ball at the center
        this->center = Vector2f(0,0);
        this->radius = 1.0;
        this->coefficientofRestituition = 1;
        this->globalToObjectTransMatrix = Matrix4f::identity();
        this->objectToGlobalTransMatrix = Matrix4f::identity();
	}

	Ball(Vector2f ballCenter, float ballRadius, float ballCOR) {
        this->center = ballCenter;
        this->radius = ballRadius;
        this->coefficientofRestituition = ballCOR;
        this->globalToObjectTransMatrix = Matrix4f::identity();
        this->objectToGlobalTransMatrix = Matrix4f::identity();
	}
	
	~Ball() {}



	virtual bool collide(Ray &r, Hit &h, float radius) {
		return false;
	}

    void addCollidable(Plane collisionItem) { this->collidable.push_back(collisionItem);
    }

    float getRadius() { return this->radius; }

    Vector2f getCenter() { return this->center; }

	void computeNextPos(float intervalTime) {
        Vector3f accDir = (this->globalToObjectTransMatrix * Vector4f(0.0f, -1.0f, 0.0f, 0.0f)).xyz();
        Vector2f acc = this->getGravity() * accDir.normalized().xy();
        // cout << "acc: ";
        //acc.print();
        //this->velocity.print();
        //this->center.print();
        Vector2f dir = intervalTime*this->velocity + 0.5 * intervalTime * intervalTime * acc;
        //cout << "dir: " << dir << " vt: " << (intervalTime * this->velocity)[1]
        //     << " at:" << (0.5 * intervalTime * intervalTime * acc)[1] << "\n";
        Ray ballPath(this->center,dir);
        Hit pathHit;
        do {
          bool collide = false;
          //cout<<"collide size:"<<this->collidable.size()<<"\n";
          for (int i = 0; i < this->collidable.size(); i++) {
            collide = collide || this->collidable[i].collide(ballPath, pathHit,this->radius);
            //cout << collide;
          }
          if (collide && pathHit.getT() < 1) {
            //cout << "checkin collision\n";
            //this->velocity.print();
            //this->center.print();
            Vector2f dist = dir * pathHit.getT();
            //dist.print();
            this->center += dist;
            Vector2f determinant = this->velocity * this->velocity + 2 * acc * dist;
            int temp = abs(acc[0]) == 0.0f ? 1 : 0;
            float timeTaken1 = 0-pow(determinant[temp], 0.5) - velocity[temp];
            timeTaken1 = timeTaken1 / acc[temp];
            float timeTaken2 = pow(determinant[temp], 0.5) - velocity[temp];
            timeTaken2 = timeTaken2 / acc[temp];
            float timeTaken = timeTaken1;
            if (timeTaken1 < 0.0f) {
              timeTaken = timeTaken2;
            } 
            else if (timeTaken2 < timeTaken1 && timeTaken2 >= 0.0f) {
              timeTaken = timeTaken2;
            }
            //cout << "timeTaken: " << timeTaken << "\n";
            this->velocity += acc * timeTaken;
            float speed = this->velocity.abs();
            this->velocity.negate();
            //this->velocity.print();
            this->velocity.normalize();
            this->velocity =
                2 * pathHit.getNormal() * Vector2f::dot(pathHit.getNormal(), this->velocity) -
                this->velocity;
            //this->velocity.print();
            this->velocity *= this->coefficientofRestituition*speed;
            //this->velocity.print();
            if (dist.abs() < 1e-21 && speed < 1e-21) {
              dir = Vector2f::ZERO;
              acc = Vector2f::ZERO;
              this->velocity = Vector2f::ZERO;
              break;
            }
            intervalTime -= timeTaken;
            pathHit.reset();
            dir = this->velocity * intervalTime + 0.5 * acc * intervalTime * intervalTime;
            ballPath.set(this->center,dir);
            //string holder;
            //cin >> holder;
          } else {
            break;
          }
          //cout <<"interavalTime:"<< intervalTime<<"\n";
        } while (intervalTime>0.0f);
        this->center += dir;
        this->velocity += acc * intervalTime;
	}

    void draw() {
          glPushMatrix();

          glMultMatrixf(Matrix4f::translation(Vector3f(this->center, 0.0f)));
          glutSolidSphere(this->radius, 12, 12);
          glPopMatrix();
    }
   

protected:
    Matrix4f globalToObjectTransMatrix;
    Matrix4f objectToGlobalTransMatrix;
    Vector2f center;
    Vector2f velocity;
    float radius;
    float coefficientofRestituition;
    vector<Plane> collidable;
};

#endif
