#pragma once
#include <vector>
#include <Solid.hpp>


static void error_callback(int error, const char* description);

void startupOpengl();

GLFWwindow* createWidnow();

void finishOpenglSetup(GLFWwindow*);

typedef struct GLVertex
{
    Vector3f pos;
    Colour3f col;
} GLVertex;

void getSolidVertices(const Solid& solid, std::vector<GLVertex>& vertices);