#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Transparent_Shader::
Transparent_Shader(const Parse* parse,std::istream& in)
{
    in>>name>>index_of_refraction>>opacity;
    shader=parse->Get_Shader(in);
    assert(index_of_refraction>=1.0);
}

// Use opacity to determine the contribution of this->shader and the Schlick
// approximation to compute the reflectivity.  This routine shades transparent
// objects such as glass.  Note that the incoming and outgoing indices of
// refraction depend on whether the ray is entering the object or leaving it.
// You may assume that the object is surrounded by air with index of refraction
// 1.
vec3 Transparent_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    Debug_Scope scope;
    double n_1, n_2;
    vec3 n;
    if(dot(ray.direction, normal) > 0)
    {
        n_1 = index_of_refraction;
        n_2 = 1;
        n = -normal;
    }
    else
    {
        n_1 = 1;
        n_2 = index_of_refraction;
        n = normal;
    }
    
    double index_ratio = n_1 / n_2;
    vec3 v = -ray.direction;
    double costheta_1 = dot(n, v);
    double sintheta_2_squared = pow(index_ratio, 2) * (1 - pow(costheta_1, 2));
    double costheta_2 = sqrt(1 - sintheta_2_squared);
    double sintheta_1 = sqrt(1 - pow(costheta_1, 2));
    double sintheta_2 = index_ratio * sintheta_1;
    vec3 b = (1 / sintheta_1) * (v - costheta_1 * n);
    vec3 t = -costheta_2 * n - sintheta_2 * b;
    Ray reflected_ray(intersection_point, ray.direction - 2 * dot(ray.direction, normal) * normal);
    Ray refracted_ray(intersection_point, t);
    vec3 reflected_color;
    if(recursion_depth >= render_world.recursion_depth_limit)
    {
        reflected_color = vec3(0, 0, 0);
    }
    else
    {
        reflected_color = render_world.Cast_Ray(reflected_ray, recursion_depth + 1);
    }
    
    if(sintheta_2_squared > 1)
    {
        vec3 object_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);
        vec3 color = object_color + reflected_color;
        return color;
    }
        
    vec3 refracted_color;
    if(recursion_depth >= render_world.recursion_depth_limit)
    {
        refracted_color = vec3(0, 0, 0);
    }
    else
    {
        refracted_color = render_world.Cast_Ray(refracted_ray, recursion_depth + 1);
    }
    double R_0 = pow((n_1 - n_2) / (n_1 + n_2), 2);
    double R = R_0 + (1 - R_0) * std::max(pow(1 - costheta_1, 5), pow(1 - costheta_2, 5));
    vec3 combined_color = R * reflected_color + (1 - R) * refracted_color;
    
    vec3 object_color = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);
    vec3 color = opacity * object_color + (1 - opacity) * combined_color;
    
    //return {};
}
