#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "World.hpp"
#include "PlayerController.hpp"

#include<BooleanModeller.hpp>


Point3f World::center(0,0,0);

std::map<std::tuple<int,int,int>,Chunk> World::chunks;

GLint World::vpos;
GLint World::vcol;
GLint World::offset;

void World::setup(GLint vpos, GLint vcol, GLint offset){
    World::vpos = vpos;
    World::vcol = vcol;
    World::offset = offset;
}

void World::update(double dt)
{
    center = PlayerController::getPosition();
    World::loadUnloadChunks();
}

void World::cleanup()
{
    chunks.clear();
}

void World::draw()
{
    for(auto& chunk: chunks)
    {
        auto key = chunk.first;
        int x = std::get<0>(key);
        int y = std::get<1>(key);
        int z = std::get<2>(key);

        if(center.distance(Vector3f({x*CHUNK_WIDTH,y*CHUNK_HEIGHT,z*CHUNK_WIDTH})) < RADIUS_VISUAL)
        {
            chunk.second.draw();
        }
    }
}

Chunk* World::getChunk(int x, int y, int z)
{
    auto it = chunks.find({x,y,z});

    if(it == chunks.end()) return nullptr;

    return &(it->second);
}

void World::loadUnloadChunks()
{
    std::vector<std::tuple<int,int,int>> deleteKeys;

    for(auto& chunk: chunks)
    {
        auto key = chunk.first;
        int x = std::get<0>(key);
        int y = std::get<1>(key);
        int z = std::get<2>(key);
        if(center.distance(Vector3f({x*CHUNK_WIDTH,y*CHUNK_HEIGHT,z*CHUNK_WIDTH})) > RADIUS_LOAD)
        {
            deleteKeys.emplace_back(key);
        }
    }

    for(auto key : deleteKeys){
        chunks.erase(key);
    }

    const static int radiusWidth = ((RADIUS_LOAD-1)/CHUNK_WIDTH)+1;
    const static int radiusHeight = ((RADIUS_LOAD-1)/CHUNK_HEIGHT)+1;

    int center_x = center.x/CHUNK_WIDTH;
    int center_y = center.y/CHUNK_HEIGHT;
    int center_z = center.z/CHUNK_WIDTH;

    for(int x = -radiusWidth; x<= radiusWidth; ++x){
        for(int y = -radiusHeight; y<= radiusHeight; ++y){
            for(int z = -radiusWidth; z<= radiusWidth; ++z){
                if((center.distance(Vector3f({x*CHUNK_WIDTH,y*CHUNK_HEIGHT,z*CHUNK_WIDTH}) + (Vector3f)center) < RADIUS_LOAD) && chunks.find({x,y,z}) == chunks.end())
                {
                    chunks.try_emplace({x+center_x, y+center_y, z+center_z},x+center_x, y+center_y, z+center_z, vpos, vcol, offset);
                }
            }
        }
    }
}

double World::edit(Solid& solid, bool place){
    Point3f point = solid.getVertices()[0];
    Bound objectBound(solid.getVertices());

    double rval = 0;
    
    int center_x = (point.x/CHUNK_WIDTH);
    int center_y = (point.y/CHUNK_WIDTH);
    int center_z = (point.z/CHUNK_WIDTH);
    
    for(int x = center_x-1; x<= (center_x+1); ++x)
        for(int y = center_y-1; y<= (center_y+1); ++y)
            for(int z = center_z-1; z<= (center_z+1); ++z){
                auto chunk = getChunk(x,y,z);
                if(chunk == nullptr) continue;

                Bound chunkBound(chunk->getBound());
                if(objectBound.overlap(chunkBound)){
                    if(place){
                        rval += chunk->place(solid);
                    }else{
                        rval += chunk->dig(solid);
                    }
                }
            }

    return rval;
}

Vector3f World::ray(Vector3f position, Vector3f direction){
    Vector3f closestPoint = {NAN,NAN,NAN};
    double closestDist = INFINITY;
    int center_x = (position.x/CHUNK_WIDTH);
    int center_y = (position.y/CHUNK_WIDTH);
    int center_z = (position.z/CHUNK_WIDTH);
    
    for(int x = center_x-1; x<= (center_x+1); ++x)
        for(int y = center_y-1; y<= (center_y+1); ++y)
            for(int z = center_z-1; z<= (center_z+1); ++z){
                auto chunk = getChunk(x,y,z);
                if(chunk == nullptr) continue;
                //fprintf(stderr,"x: %d,y: %d,z: %d",x,y,z);
                
                Vector3f curr_point = chunk->ray(position,direction);
                if(!curr_point.isNAN() && (position-curr_point).length()<closestDist){
                    closestPoint = curr_point;
                    closestDist = (position-curr_point).length();
                }

            }

    return closestPoint;
}

Solid World::collide(Solid& object){
    Point3f point = object.getVertices()[0];
    Bound objectBound(object.getVertices());

    Solid current_intersection;
    int center_x = (point.x/CHUNK_WIDTH);
    int center_y = (point.y/CHUNK_WIDTH);
    int center_z = (point.z/CHUNK_WIDTH);
    
    for(int x = center_x-1; x<= (center_x+1); ++x)
        for(int y = center_y-1; y<= (center_y+1); ++y)
            for(int z = center_z-1; z<= (center_z+1); ++z){
                auto chunk = getChunk(x,y,z);
                if(chunk == nullptr) continue;
                
                Bound chunkBound(chunk->getBound());
                if(objectBound.overlap(chunkBound)){

                    BooleanModeller modeller(current_intersection,chunk->collides(object));
                    current_intersection = modeller.getUnion();
                }
            }

    return current_intersection;
}