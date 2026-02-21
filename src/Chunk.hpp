#pragma once

#include<Solid.hpp>
class Bound;
class VAO;


class Chunk
{
public:
    Chunk(int x, int y, int z, GLint vpos_location, GLint vcol_location, GLint offset_location);
    ~Chunk();

    double dig(Solid& other);
    double place(Solid& other);

    Solid collides(Solid& other);
    Bound getBound();

    Vector3f ray(Vector3f position, Vector3f direction);

    void draw();

private:
    void calculateVAO();

//chunk location (center of each chunk is 0,0,0 for its own location)
    int x,y,z;

    Solid boundingBox;
    Solid ground;

    VAO* VertexObject;

    constexpr static const float CHUNK_HEIGHT = 16;
    constexpr static const float CHUNK_WIDTH = 8;

    GLint vpos;
    GLint vcol;
    GLint offset;
};


