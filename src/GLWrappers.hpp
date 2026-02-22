#pragma once
#include <vector>
#include <Solid.hpp>
#include <linmath.h>

static void error_callback(int error, const char* description);

void startupOpengl();

GLFWwindow* createWidnow();

void finishOpenglSetup(GLFWwindow*);

typedef class GLVertex
{
public:
    vec3 pos;
    vec3 col;
    GLVertex(Point3f a, Colour3f c){
        pos[0] = float(a.x);
        pos[1] = float(a.y);
        pos[2] = float(a.z);
        col[0] = float(c.r);
        col[1] = float(c.g);
        col[2] = float(c.b);
    }

    GLVertex(){}

    float& x(){return pos[0];}
    float& y(){return pos[1];}
    float& z(){return pos[2];}
    float& r(){return col[0];}
    float& g(){return col[1];}
    float& b(){return col[2];}

    const float& x()const{return pos[0];}
    const float& y()const{return pos[1];}
    const float& z()const{return pos[2];}
    const float& r()const{return col[0];}
    const float& g()const{return col[1];}
    const float& b()const{return col[2];}
} GLVertex;

void getSolidVertices(const Solid& solid, std::vector<GLVertex>& vertices);