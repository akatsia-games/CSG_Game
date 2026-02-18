#pragma once
#include <Point3f.hpp>

static void error_callback(int error, const char* description);

void startupOpengl();

GLFWwindow* createWidnow();

void finishOpenglSetup(GLFWwindow*);

typedef struct GLVertex
{
    Vector3f pos;
    Colour3f col;
} GLVertex;