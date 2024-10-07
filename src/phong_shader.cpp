// Student Name: Austin Haugland
// Student ID: 862293525
#include "light.h"
#include "parse.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    Debug_Scope scope;
    std::pair<Shaded_Object,Hit> h;
    vec3 color;
    vec3 Amb;
    vec3 diffuse;
    vec3 specular;
    vec3 R, r_c;
    if(render_world.ambient_color && color_ambient)
        {
        Amb = this->color_ambient->Get_Color(hit.uv) * render_world.ambient_intensity * render_world.ambient_color->Get_Color({});
        }
    for(unsigned int i = 0; i < render_world.lights.size(); i++){
        vec3 light_dir = render_world.lights[i]->position - intersection_point; //Position of light at given and it's Light direction
        Ray light_to_IP(intersection_point, light_dir); //Ray of light to Intersection point
        h = render_world.Closest_Intersection(light_to_IP); //Declaration of Closest_Intersection
        double light_dist = sqrt(pow(light_dir[0], 2.0) + pow(light_dir[1], 2.0) + pow(light_dir[2], 2.0)); //Distance of light (necessary for test 19) 
        R = render_world.lights[i]->Emitted_Light(light_dir); //Emitted Light in a given direction
        vec3 d = light_dir.normalized();
        vec3 lc = render_world.lights[i]->Emitted_Light(light_dir)/light_dir.magnitude_squared();
        if((h.second.dist >= small_t && light_dist > h.second.dist) && render_world.enable_shadows) //Checks if shadow is on
        {
            //regardless
            //continue;
        }
        else
        {
            //Color Diffuse
            double intense = std::max(0.0, dot(light_dir.normalized(), normal));
            double magn = dot(normal, (light_dir).normalized());
            if(magn < 0)
            {
                magn = 0;
            }
            diffuse = this->color_diffuse->Get_Color(hit.uv) * R * magn;

            //Color Specular
            r_c = (2.0 * dot((light_dir).normalized(), normal) * normal) - (light_dir).normalized();
            magn = dot(r_c, (render_world.camera.position-intersection_point).normalized());
            if(magn <= 0)
            {
                magn = 0;
            }
            magn = pow(magn, specular_power);
            specular = this->color_specular->Get_Color(hit.uv) * R * magn;
            color = color + diffuse + specular;
        }
    }
    color = color + Amb;
    return color;
}