#include "mesh.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

Mesh::Mesh(const Parse* parse, std::istream& in)
{
    std::string file;
    in>>name>>file;
    Read_Obj(file.c_str());
}

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e, t;
    vec3 v;
    vec2 u;
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }

        if(sscanf(line.c_str(), "vt %lg %lg", &u[0], &u[1]) == 2)
        {
            uvs.push_back(u);
        }

        if(sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &e[0], &t[0], &e[1], &t[1], &e[2], &t[2]) == 6)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
            for(int i=0;i<3;i++) t[i]--;
            triangle_texture_index.push_back(t);
        }
    }
    num_parts=triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    //TODO;
    Hit CH;
    double t = std::numeric_limits<double>::max();
    for(int i = 0; i < triangles.size(); ++i){
        Hit hit = Intersect_Triangle(ray, i);
        if(hit.Valid() && hit.dist < t){
            CH = hit;
            t = hit.dist;
        }
    }
    return CH;
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    assert(hit.triangle>=0);
    ivec3 cur_triangle = triangles[hit.triangle];
    vec3 A = vertices[cur_triangle[0]];
    vec3 B = vertices[cur_triangle[1]];
    vec3 C = vertices[cur_triangle[2]];
    vec3 N = cross((A-B), (A-C)).normalized();
    return N;
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{
    Debug_Scope scope;
    Hit hit;
    hit.dist = std::numeric_limits<double>::infinity();
    //hit.triangle = tri;
    ivec3 cur_triang = triangles[tri];
    vec3 A = vertices[cur_triang[0]];
    vec3 B = vertices[cur_triang[1]];
    vec3 C = vertices[cur_triang[2]];
    //vec3 IPT = ray.Point(hit.dist);
    vec3 u = C - A;
    vec3 v = B - A;
    
    vec3 rayW = ray.endpoint - A;
    vec3 n = cross(v, u).normalized();
    double denominator = dot(n, ray.direction);
    if(abs(denominator) < small_t){
        return hit;
    }
    double dist = -dot(n, rayW) / denominator;
    if(dist < small_t || dist > hit.dist){
        return hit;
    }
    vec3 IP = ray.Point(dist);
    vec3 y = IP - A;

    double denominator2 = dot(cross(u,v), n);
    double Gamma = dot(cross(u, y), n)/ denominator2;
    double Beta = dot(cross(y, v), n)/ denominator2;
    double Alpha = 1 - Beta - Gamma;

    if(Alpha > -weight_tol && Beta > -weight_tol && Gamma > -weight_tol){
        hit.dist = dist;
        hit.triangle = tri;
        return hit;
    }
    return hit;
}

std::pair<Box,bool> Mesh::Bounding_Box(int part) const
{
    if(part<0)
    {
        Box box;
        box.Make_Empty();
        for(const auto& v:vertices)
            box.Include_Point(v);
        return {box,false};
    }

    ivec3 e=triangles[part];
    vec3 A=vertices[e[0]];
    Box b={A,A};
    b.Include_Point(vertices[e[1]]);
    b.Include_Point(vertices[e[2]]);
    return {b,false};
}
