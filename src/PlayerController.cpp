#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "PlayerController.hpp"
#include "Point3f.hpp"
#include <chrono>
#include <iostream>

Vector3f PlayerController::position({0,0,0});

double PlayerController::horAngle(0);
double PlayerController::verAngle(0);

mat4x4 PlayerController::MVP;

bool PlayerController::ignoreInputs(true);

std::bitset<6> PlayerController::mv_keys;

GLFWwindow* PlayerController::window(nullptr);

int PlayerController::width(640);
int PlayerController::height(480);

double getSeconds(){
    auto time = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    return millis/1000.0;
}

void PlayerController::initialize(GLFWwindow* window)
{
    position = {0,0,3};

    horAngle = 0;
    verAngle = 0;

    ignoreInputs = false;

    PlayerController::window = window;

    calculateMatrix();
    glfwSetInputMode(PlayerController::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(PlayerController::window, cursor_position_callback);
    glfwSetWindowFocusCallback(PlayerController::window, window_focus_callback);
 
    glfwSetKeyCallback(PlayerController::window, key_callback);
}

const GLfloat* PlayerController::mvp()
{
    return (const GLfloat*)&PlayerController::MVP;
}

float ratio = PlayerController::width/PlayerController::height;

float multiplier = M_PI/PlayerController::height;

double lastPositionCallback = 0;

void PlayerController::calculateMatrix(){
    mat4x4 T,R,P ,h, v, id, TR;

    mat4x4_translate(T,
        -position.x,
        -position.y,
        -position.z);

    mat4x4_identity(id);

    mat4x4_rotate_Y(h,id,-horAngle);
    mat4x4_rotate_X(v,id,-verAngle);
    
    //full rotational matrix
    mat4x4_mul(R,v,h);
    
    //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_perspective(P,1,ratio,1e-5,1e5);

    mat4x4_mul(TR, R, T);
    
    mat4x4_mul(MVP, P, TR);
}

double xlast,ylast;
void PlayerController::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(ignoreInputs) return;
    double curr_time = getSeconds();

    double elapsed_time = curr_time-lastPositionCallback;
    lastPositionCallback = curr_time;

    double xdiff = xlast - xpos;
    double ydiff = ylast - ypos;

    xlast = xpos;
    ylast = ypos;

    if(std::abs(elapsed_time)>1) return;

    verAngle += ydiff*multiplier;
    verAngle = std::min(std::max(verAngle,-7.0 * M_PI/16), M_PI*7.0/16);

    horAngle = std::fmod(horAngle+(xdiff*multiplier),(M_PI*2));
}

void PlayerController::window_focus_callback(GLFWwindow* window, int focused)
{
    if (focused)
    {
        mv_keys.reset();
        ignoreInputs = false;
    }
    else
    {
        ignoreInputs = true;
    }
}

void PlayerController::update(double dt){

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);

    if(ignoreInputs)
    {
        calculateMatrix();
        return;
    }
    int up(0), right(0), back(0);

    up +=  mv_keys[Key::UP];
    up -=  mv_keys[Key::DOWN];
    right+=mv_keys[Key::RIGHT];
    right-=mv_keys[Key::LEFT];
    back -= mv_keys[Key::FORWARD];
    back += mv_keys[Key::BACK];

    if(up == 0 && right == 0 && back == 0)
    {
        calculateMatrix();
        return;
    }

    Vector3f direction{(float)right,(float)up,(float)back};
    direction.normalize();

    vec4 look_dir{direction.x,direction.y,direction.z,0};
    vec4 look_dir_cpy{direction.x,(float)up,direction.z,0};

    mat4x4 h, id;
    mat4x4_identity(id);

    mat4x4_rotate_Y(h,id,horAngle);

    mat4x4_mul_vec4(look_dir, h, look_dir_cpy);

    direction = {look_dir[0],look_dir[1],look_dir[2]};
    direction*= dt;

    PlayerController::position += direction;

    calculateMatrix();
}

void PlayerController::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (!(action==GLFW_PRESS || action == GLFW_RELEASE))
        return;
    switch (key)
    {
    case GLFW_KEY_ESCAPE:
        if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);       
    case GLFW_KEY_UP:
        mv_keys[Key::UP] = (action == GLFW_PRESS);
        break;
    case GLFW_KEY_DOWN:
        mv_keys[Key::DOWN] = (action == GLFW_PRESS);
        break;
    case GLFW_KEY_D:
        mv_keys[Key::RIGHT] = (action == GLFW_PRESS);
        break;
    case GLFW_KEY_A:
        mv_keys[Key::LEFT] = (action == GLFW_PRESS);
        break;
    case GLFW_KEY_W:
        mv_keys[Key::FORWARD] = (action == GLFW_PRESS);
        break;
    case GLFW_KEY_S:
        mv_keys[Key::BACK] = (action == GLFW_PRESS);
        break;
    default:
        break;
    }
}
 