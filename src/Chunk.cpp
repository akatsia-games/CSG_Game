#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <BooleanModeller.hpp>
#include <fstream>

#include "Chunk.hpp"
#include "GLWrappers.hpp"
#include "VAO.hpp"

Chunk::Chunk(int x, int y, int z, GLint vpos_location, GLint vcol_location, GLint offset_location)
    :VertexObject(nullptr)
{
    vpos = vpos_location;
    vcol = vcol_location;
    offset = offset_location;

    std::ifstream solidFile("../resources/objects/cube.obj");

    this->x = x;
    this->y = y;
    this->z = z;

    Vector3f colour = {(float)sin(x*M_PI/8),(float)sin(y*M_PI/8),(float)sin(z*M_PI/8)};
    colour+=(Vector3f({1.1,1.1,1.1}));
    colour.normalize();

    if(x==y && y==z && x==0)
    {
        fprintf(stderr,"\n(%f,%f,%f)\n",colour.x,colour.y,colour.z);
    }

    boundingBox = Solid(solidFile,{colour.x, colour.y, colour.z});
    boundingBox.translate(-0.5,-0.5,-0.5);

    boundingBox.scale(CHUNK_WIDTH, CHUNK_HEIGHT, CHUNK_WIDTH);
    
    if(y == 0){
        ground = Solid(boundingBox);
        ground.scale(1,2/CHUNK_HEIGHT,1);
        calculateVAO();
    }
}

Chunk::~Chunk(){
    if(VertexObject != nullptr) delete VertexObject;
}

void Chunk::calculateVAO()
{
    if(VertexObject != nullptr){
        delete VertexObject;
        VertexObject = nullptr;
    } 
    if(ground.isEmpty()) return;

    std::vector<GLVertex> vertices;
    getSolidVertices(ground, vertices);

    
    VertexObject = new VAO(vertices, vpos, vcol);
}

void Chunk::draw()
{
    if(VertexObject == nullptr) return;
    
    glUniform3f(offset, x*CHUNK_WIDTH, y*CHUNK_HEIGHT, z*CHUNK_WIDTH);
    VertexObject->draw();

    //fprintf(stderr,"draw object at (%f,%f,%f)",)
}

Solid Chunk::collides(Solid& other)
{
    BooleanModeller colliderCheck(this->ground, other);
    return colliderCheck.getIntersection();
}


double Chunk::dig(Solid& other)
{
    BooleanModeller perimeter(boundingBox, other);

    BooleanModeller modeller(ground, perimeter.getIntersection());
    ground = modeller.getDifference();

    calculateVAO();

    return modeller.getIntersection().getVolume();
}

double Chunk::place(Solid& other)
{
    BooleanModeller perimeter(boundingBox, other);

    BooleanModeller modeller(ground, perimeter.getIntersection());
    ground = modeller.getUnion();
    
    calculateVAO();

    return modeller.getDifference().getVolume();
}

Bound Chunk::getBound(){
    if(ground.isEmpty()){
        return Bound();
    }
    return Bound(ground.getVertices());
}

Vector3f Chunk::ray(Vector3f position, Vector3f direction){
    return ground.intersectRay(position, direction);
}