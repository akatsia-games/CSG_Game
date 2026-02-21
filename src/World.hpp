#pragma once
#include "Chunk.hpp"
#include <tuple>
#include <map>

class World{
public:
    static void setup(GLint vpos, GLint vcol, GLint offset);

    static void update(double dt);

    static void draw();

    static Chunk* getChunk(int x, int y, int z);

    static double edit(Solid& solid, bool place);

    static Vector3f ray(Vector3f position, Vector3f direction);

    static Solid collide(Solid& object);

    static void cleanup();

private:
//center, updated from Player's position
    static Point3f center;

    static void loadUnloadChunks();

    static std::map<std::tuple<int,int,int>,Chunk> chunks;

    constexpr static const float RADIUS_LOAD = 100;
    constexpr static const float RADIUS_VISUAL = 80;

    constexpr static const float CHUNK_HEIGHT = 16;
    constexpr static const float CHUNK_WIDTH = 8;

    static GLint vpos;
    static GLint vcol;
    static GLint offset;
};