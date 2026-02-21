#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "PlayerController.hpp"
#include "Point3f.hpp"
#include <chrono>
#include <iostream>
#include "World.hpp"
#include <algorithm>
#include <fstream>

Solid loadCube(Vector3f point, double width, double height, double y_angle = 0)
{
    std::ifstream cube_file("../resources/objects/cube.obj");
    Solid cube(cube_file,{0.5,0.5,1});

    cube.translate(-0.5,-0.5,-0.5);

    cube.scale(width,height,width);
    cube.rotate(0,y_angle);

    cube.translate(point.x,point.y,point.z);

    return cube;
}

Vector3f PlayerController::position({0,0,0});
Vector3f PlayerController::velocity({0,0,0});

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
    position = {0,1+1e-2,0};

    horAngle = (M_PI/4);
    verAngle = -(M_PI/8);

    ignoreInputs = false;

    PlayerController::window = window;

    calculateMatrix();
    glfwSetInputMode(PlayerController::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(PlayerController::window, cursor_position_callback);
    glfwSetWindowFocusCallback(PlayerController::window, window_focus_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
 
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
    mat4x4_perspective(P,1,ratio,1e-2,1e8);

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
    //fprintf(stderr,"(%f, %f, %f)\n", position.x, position.y, position.z);

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float) height;

    glViewport(0, 0, width, height);

    if(ignoreInputs)
    {
        calculateMatrix();
        return;
    }
    int right(0), back(0);

    right+=mv_keys[Key::RIGHT];
    right-=mv_keys[Key::LEFT];
    back -= mv_keys[Key::FORWARD];
    back += mv_keys[Key::BACK];

    velocity *= std::pow(0.25,dt);
    
    if(!mv_keys[SPACE] && right == 0 && back == 0 && velocity.length()<1e-5 && isOnGround())
    {
        calculateMatrix();
        return;
    }

    Vector3f direction{(float)right, 0,(float)back};
    if(direction.length()>1e-5){
        direction.normalize();
    }

    vec4 look_dir{direction.x, 0, direction.z, 0};
    vec4 look_dir_cpy{direction.x, 0, direction.z, 0};

    mat4x4 h, id;
    mat4x4_identity(id);

    mat4x4_rotate_Y(h,id,horAngle);

    mat4x4_mul_vec4(look_dir, h, look_dir_cpy);

    direction = {look_dir[0],look_dir[1],look_dir[2]};

    velocity += direction*4*dt;

    if(isOnGround())
    {
        velocity.y = std::max(0.0f,velocity.y);
        if(mv_keys[SPACE])
            velocity.y += 9.8;
    }else{
        velocity.y -= 9.8 * dt;
    }

    float last_y = position.y;
    PlayerController::position += velocity*dt;

    Solid object = loadCube(position+Vector3f({0,-0.6,0}), 0.3,1.8);

    Solid intersection;// = World::collide(object);

    if(intersection.isEmpty()){
        calculateMatrix();
        return;
    }
    
    float heighest_point = 1.0;
    for(auto point: intersection.getVertices()){
        if(point.y>heighest_point) heighest_point = point.y;
    }

    position.y = std::max( std::min(heighest_point, last_y), position.y);
    velocity.y = 0;

    calculateMatrix();
}

bool PlayerController::isOnGround(){
    return position.y<(1 + 1.5 + 1e-5);

    Solid object = loadCube(position-Vector3f({0,-1e-5 - 0.6,0}), 0.3,1.8);  
    object = World::collide(object);
    
    return !object.isEmpty();
}

Point3f PlayerController::getPosition(){
    return (Point3f)position;
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
    case GLFW_KEY_SPACE:
        mv_keys[Key::SPACE] = (action == GLFW_PRESS);
        break;
    case GLFW_KEY_LEFT_CONTROL:
    case GLFW_KEY_RIGHT_CONTROL:
        mv_keys[Key::CONTROL] = (action == GLFW_PRESS);
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
 
void PlayerController::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if ((button == GLFW_MOUSE_BUTTON_LEFT||button==GLFW_MOUSE_BUTTON_RIGHT) && action == GLFW_PRESS)
    {
        Vector3f look_dir = {0,0,-1};
        look_dir.rotate({1,0,0}, -verAngle);
        look_dir.rotate({0,1,0}, -horAngle);

        Vector3f intersection = World::ray(position, look_dir);

        //fprintf(stderr,"attempting to place/dig(%d) at: %s\n",(button==GLFW_MOUSE_BUTTON_RIGHT),intersection.toString().c_str());

        if(intersection.isNAN()) return;
        if((intersection-position).length() > 10) return;

        Solid placeableSolid = loadCube(intersection, 1,1, PlayerController::horAngle);

        World::edit(placeableSolid, button==GLFW_MOUSE_BUTTON_RIGHT);
    }
        
}