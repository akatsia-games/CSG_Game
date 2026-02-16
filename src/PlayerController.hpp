#pragma once
#include <bitset>
#include <linmath.h>

class GLFWwindow;
class Vector3f;


double getSeconds();

class PlayerController{
private:

    enum Key{
        UP = 0,
        DOWN,
        LEFT,
        RIGHT,
        FORWARD,
        BACK
    };

public:
    static int width;
    static int height;

    static void initialize(GLFWwindow* window);

    const static GLfloat* mvp();

    static void update(double dt);


private:
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    
    static void window_focus_callback(GLFWwindow* window, int focused);

    static void calculateMatrix();

    static Vector3f position;

    static double horAngle,verAngle;

    static mat4x4 MVP;

    static bool ignoreInputs;

    static std::bitset<6> mv_keys;

    static GLFWwindow* window;
};