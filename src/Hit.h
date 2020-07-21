#ifndef HIT_H
#define HIT_H

#include "vecmath.h"
#include <float.h>
#include<ostream>


class Hit
{
public:


    
    // constructors
    Hit()
    {
		t = FLT_MAX;
    }

    Hit( float _t, const Vector2f& n )
    { 
        t = _t;
        normal = n;
    }

    Hit( const Hit& h )
    { 
        t = h.t; 
        normal = h.normal;
    }

    // destructor
    ~Hit()
    {
    
    }

    float getT() const
    {
        return t;
    }

    Vector2f getNormal() const {
        return normal;
    }

    void reset() { t = FLT_MAX;}

    void set( float _t, const Vector2f& n )
    {
        t = _t;
        normal = n;
    }
    
private:
	float t;
    Vector2f normal;

};

inline std::ostream& operator << ( std::ostream &os, const Hit& h)
{
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
