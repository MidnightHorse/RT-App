// Student Name: Austin Haugland
// Student ID: 862293525

#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>

Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

// Intersect with the plane.  The plane's normal points outside.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    Hit PlaneH{0, -1};
    double udot = dot(ray.direction, normal);

    if(!udot)
    {
        return PlaneH;
    }
    double t = (dot((this->x - ray.endpoint), this->normal)/
    dot(ray.direction, this->normal));
    if(t < small_t)
    {
        return PlaneH;
    }
    else
    {
        PlaneH.dist = t;
        return PlaneH;
    }
}

vec3 Plane::Normal(const Ray& ray, const Hit& hit) const
{
    return normal;
}

std::pair<Box,bool> Plane::Bounding_Box(int part) const
{
    Box b;
    b.Make_Full();
    return {b,true};
}
