#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "PlayerController.hpp"
#include "GLWrappers.hpp"
#include "GLProgram.hpp"
#include "VAO.hpp"

std::vector<Vertex> vertices =
{
    { { -0.6f, -0.4f , 1.0f}, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f , 1.0f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f , 1.0f }, { 0.f, 0.f, 1.f } }
}; 
 
int main(void)
{
    startupOpengl();
    GLFWwindow* window = createWidnow();

    PlayerController::initialize(window);
 
    finishOpenglSetup(window);
 
    GLProgram program("../src/vertex.glsl","../src/fragment.glsl");

    const GLint mvp_location = glGetUniformLocation(program.pointer(), "MVP");
    const GLint vpos_location = glGetAttribLocation(program.pointer(), "vPos");
    const GLint vcol_location = glGetAttribLocation(program.pointer(), "vCol");

    VAO vao(vertices, vpos_location, vcol_location);

    double curr, past;
    curr = getSeconds();

    while (!glfwWindowShouldClose(window))
    {
        past = curr;
        curr = getSeconds();
        double dt = curr-past;

        PlayerController::update(dt);

        glClear(GL_COLOR_BUFFER_BIT);

        program.use();
        
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, PlayerController::mvp());
        vao.draw();
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}