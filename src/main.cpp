#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "PlayerController.hpp"
#include "GLWrappers.hpp"
#include "GLProgram.hpp"
#include "VAO.hpp"
#include "World.hpp"

#include <fstream>

#include <Solid.hpp>
#include <BooleanModeller.hpp>

std::ifstream solidFile("../resources/objects/cube.obj");
Solid baseSolid(solidFile,{1,0,0});

Solid solidFloor(baseSolid);
Solid floorCpy;

VAO* active_vao(nullptr);

GLint vpos_location;
GLint vcol_location;
 
int main(void)
{
    solidFloor.translate(-0.5,-1,-0.5);
    solidFloor.scale(10,3,10);
    floorCpy = solidFloor;

    startupOpengl();
    GLFWwindow* window = createWidnow();

    PlayerController::initialize(window);
 
    finishOpenglSetup(window);
 
    GLProgram program("../resources/shaders/vertex.glsl","../resources/shaders/fragment.glsl");

    const GLint mvp_location = glGetUniformLocation(program.pointer(), "MVP");
    const GLint pos_location = glGetUniformLocation(program.pointer(), "oPos");
    vpos_location = glGetAttribLocation(program.pointer(), "vPos");
    vcol_location = glGetAttribLocation(program.pointer(), "vCol");

    fprintf(stderr,"%d",pos_location);

    World::setup(vpos_location,vcol_location,pos_location);

    std::vector<GLVertex> vertices;
    getSolidVertices(solidFloor, vertices);
    active_vao = new VAO(vertices, vpos_location, vcol_location);

    double curr, past;
    curr = getSeconds();

    glEnable(GL_CULL_FACE);

    //Enable depth buffering
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window))
    {
        past = curr;
        curr = getSeconds();
        double dt = curr-past;

        PlayerController::update(dt);
        World::update(dt);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, PlayerController::mvp());
        //active_vao->draw();
        World::draw();
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    World::cleanup();
 
    glfwDestroyWindow(window);
 
    delete active_vao;
    glfwTerminate();
    exit(EXIT_SUCCESS);
}