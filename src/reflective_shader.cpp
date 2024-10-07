#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    std::pair<Shaded_Object,Hit> MERGED;
    vec3 cr;
    vec3 co;
    co = shader->Shade_Surface(render_world, ray, MERGED.second, intersection_point, normal, recursion_depth);
    vec3 rayd = 2 * dot(-ray.direction, normal) * normal + ray.direction;
    Ray r(intersection_point, rayd);
    if(recursion_depth >= render_world.recursion_depth_limit){
        vec3 ret;
        ret = (1-reflectivity) * co;
        return ret;
    }
    cr = render_world.Cast_Ray(r, ++recursion_depth);
    co = (1-reflectivity)*co + (reflectivity * cr); //Given in lecture slide 6
    return co;
}
