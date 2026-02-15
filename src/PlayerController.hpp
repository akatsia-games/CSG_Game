#pragma once

class GLFWwindow;


class PlayerController{
public:

    static void initialize(GLFWwindow* window);

    static GLfloat* mvp();

    static void update(double dt);


private:
    
    static GLFWwindow* window;
};