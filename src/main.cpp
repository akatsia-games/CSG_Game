#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "PlayerController.hpp"
#include "GLWrappers.hpp"
#include "GLProgram.hpp"
#include "VAO.hpp"

#include <fstream>

#include <Solid.hpp>
#include <BooleanModeller.hpp>

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

std::ifstream solidFile("../resources/objects/cube.obj");
Solid baseSolid(solidFile,{1,0,0});

Solid solidFloor(baseSolid);
Solid floorCpy;

VAO* active_vao(nullptr);

GLint vpos_location;
GLint vcol_location;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if ((button == GLFW_MOUSE_BUTTON_LEFT||button==GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS)
    {
        Solid placeableSolid(baseSolid);
        for(Colour3f& col: placeableSolid.getColors()){
            col = {0,1,1};
        }
        
        placeableSolid.translate(4.5,-0.5,4.5);

        BooleanModeller modeller(solidFloor, placeableSolid);
        if(button == GLFW_MOUSE_BUTTON_LEFT){
            solidFloor = modeller.getDifference();
        }else{
            solidFloor = modeller.getUnion();
        }
        

        std::vector<GLVertex> vertices;
        getSolidVertices(solidFloor, vertices);

        delete active_vao;
        active_vao = new VAO(vertices, vpos_location, vcol_location);
    }
        
}
 
int main(void)
{
    solidFloor.translate(-0.5,-1,-0.5);
    solidFloor.scale(10,3,10);
    floorCpy = solidFloor;

    startupOpengl();
    GLFWwindow* window = createWidnow();

    PlayerController::initialize(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
 
    finishOpenglSetup(window);
 
    GLProgram program("../resources/shaders/vertex.glsl","../resources/shaders/fragment.glsl");

    const GLint mvp_location = glGetUniformLocation(program.pointer(), "MVP");
    vpos_location = glGetAttribLocation(program.pointer(), "vPos");
    vcol_location = glGetAttribLocation(program.pointer(), "vCol");

    std::vector<GLVertex> vertices;
    getSolidVertices(solidFloor, vertices);
    active_vao = new VAO(vertices, vpos_location, vcol_location);

    double curr, past;
    curr = getSeconds();

    glEnable(GL_CULL_FACE);
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        program.use();

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, PlayerController::mvp());
        active_vao->draw();
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    delete active_vao;
    glfwTerminate();
    exit(EXIT_SUCCESS);
}