// Student Name: Austin Haugland
// Student ID: 862293525

#include "sphere.h"
#include "ray.h"

Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    Hit intersection = {0, 0};
    vec3 HIA = ray.endpoint-center;
    double temp1 = 0;
    //double temp2 = 0;
    double a = dot(ray.direction, ray.direction);
    double b = 2 * dot(ray.direction, HIA);
    double c = dot(HIA, HIA) - (radius * radius);
    double discrim = b*b - 4*a*c;
    if(discrim >= 0){
        temp1 = (-1*b - sqrt(discrim))/(2*a);
        intersection.dist = temp1;
    }
    return intersection;
}

vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 normal;
    normal = (ray.Point(hit.dist) - center).normalized();
    return normal;
}

std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}
