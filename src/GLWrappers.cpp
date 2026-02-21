#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "GLWrappers.hpp"

void getSolidVertices(const Solid& solid, std::vector<GLVertex>& vertices){
    vertices.resize(0);
    vertices.reserve(solid.getIndices().size());

    auto& indices = solid.getIndices();
    auto& colours = solid.getColors();
    auto& verts = solid.getVertices();

    for(auto index : indices){
        GLVertex vert = {(Vector3f) verts[index],colours[index]};
        vertices.push_back(vert);
    }
}
 
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void startupOpengl()
{
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

GLFWwindow* createWidnow()
{
    glfwWindowHint(GLFW_DEPTH_BITS,24);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    return window;
}

void finishOpenglSetup(GLFWwindow* window)
{
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
}