// Student Name: Austin Haugland
// Student ID: 862293525

#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    Debug_Scope scope;
    Hit Closest_H;
    Shaded_Object SO;
    Hit hits;
    double min_t = std::numeric_limits<double>::max();
    for(auto o : objects)
    {        
        hits = o.object->Intersection(ray, 0);
        if(o.object && hits.dist < min_t && hits.dist >= small_t){
            Closest_H = hits;
            Closest_H.triangle = -1;
            min_t = hits.dist;
            SO = o;
            Pixel_Print("intersect test with ", SO.object->name, "; hit: ", min_t, " triangle: ",Closest_H.triangle, " uv: ", Closest_H.uv, ")");
        }
        else{
            Pixel_Print("No intersection with ", o.object->name);
        }
    }
    Pixel_Print("Closest intersection; obj: ", SO.object->name, " hit: ", min_t, " triangle: ",Closest_H.triangle, " uv: ", Closest_H.uv, ")");
    return {SO, Closest_H}; //{Object Hit, and Closest Hit};
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    Debug_Scope scope;
    Ray ray;
    ray.endpoint = camera.position;
    ray.direction = (camera.World_Position(pixel_index) - ray.endpoint).normalized();
    Pixel_Print("cast ray: ", ray.endpoint, " dir:", ray.direction);
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
    Debug_Scope scope;
    vec3 color;
    vec3 IP;
    vec3 N;
    std::pair<Shaded_Object,Hit> MERGED;
    MERGED = this->Closest_Intersection(ray);
    if(MERGED.first.object != NULL)
    {
        IP = ray.Point(MERGED.second.dist);
        N = MERGED.first.object->Normal(ray, MERGED.second);
        color = MERGED.first.shader->Shade_Surface(*this, ray, MERGED.second, IP, N, recursion_depth);
        Pixel_Print("Call shade surface with location: ", IP, " normal: ", N);  
        Pixel_Print("flat shader; color: ", color);
    }
    else
    {
        if(background_shader == NULL)
        {
            color = {0, 0, 0};
        }
        else
        {
            color = background_shader->Shade_Surface(*this, ray, MERGED.second, color, color, recursion_depth);
        }
    }
    return color;
}
