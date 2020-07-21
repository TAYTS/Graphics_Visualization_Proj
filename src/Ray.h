#ifndef RAY_H
#define RAY_H

#include <cassert>
#include <ostream>
#include "Vector2f.h"

using namespace std;

class Ray
{
public:

    Ray( const Vector2f& orig, const Vector2f& dir )
    {
        origin = orig; 
        direction = dir;
    }

    Ray( const Ray& r )
    { 
        origin = r.origin;
        direction = r.direction;
    }

    void set(Vector2f &orig, Vector2f &dir) {
      this->origin = orig;
      this->direction = dir;
    }

    const Vector2f& getOrigin() const
    {
        return origin;
    }

    const Vector2f& getDirection() const
    {
        return direction;
    }
    
    Vector2f pointAtParameter( float t ) const
    {
        return origin + direction * t;
    }

private:

    // don't use this constructor
    Ray()
    {
        assert( false );
    }

    Vector2f origin;
    Vector2f direction;

};

inline ostream& operator << ( ostream& os, const Ray& r)
{
    os << "Ray <" << r.getOrigin() << ", " << r.getDirection() << ">";
    return os;
}

#endif // RAY_H
