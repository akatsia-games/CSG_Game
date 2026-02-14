#include <iostream>
#include <Point3f.hpp>
#include <ctime>
#include <thread>

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include <linmath.h>
 
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <vector>
#include <bitset>
#include <chrono>

double getSeconds(){
    auto time = std::chrono::system_clock::now().time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
    return millis/1000.0;
}

int width = 640;
int height = 480;

float ratio = width/height;

float multiplier = M_PI/height;

Vector3f position{0,0,3};

double horAngle(0),verAngle(0);
double lastPositionCallback = 0;

mat4x4 mvp;

void calculateMatrix(){
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
    
    mat4x4_mul(mvp, P, TR);
}

bool ignoreInputs;

double xlast,ylast;
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
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
 
std::bitset<6> mv_keys;

void window_focus_callback(GLFWwindow* window, int focused)
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



enum Key{
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    FORWARD,
    BACK
};

static void calculatePosition(double dt){
    if(ignoreInputs) return;
    int up(0), right(0), back(0);

    up +=  mv_keys[Key::UP];
    up -=  mv_keys[Key::DOWN];
    right+=mv_keys[Key::RIGHT];
    right-=mv_keys[Key::LEFT];
    back -= mv_keys[Key::FORWARD];
    back += mv_keys[Key::BACK];

    if(up == 0 && right == 0 && back == 0) return;

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

    position += direction;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
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

typedef struct Vertex
{
    Vector3f pos;
    Colour3f col;
} Vertex;
 
static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f , 1.0f}, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f , 1.0f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f , 1.0f }, { 0.f, 0.f, 1.f } }
};
 
static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec3 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";
 
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
 
int main(void)
{
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    calculateMatrix();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowFocusCallback(window, window_focus_callback);
 
    glfwSetKeyCallback(window, key_callback);
 
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
 
    // NOTE: OpenGL error checks have been omitted for brevity
 
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
 
    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
 
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
 
    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint vcol_location = glGetAttribLocation(program, "vCol");
 
    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, pos));

    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, col));
 

    glfwGetFramebufferSize(window, &width, &height);
    calculateMatrix();

    double curr, past;
    curr = getSeconds();

    while (!glfwWindowShouldClose(window))
    {
        past = curr;
        curr = getSeconds();
        double dt = curr-past;

        calculatePosition(dt);
        calculateMatrix();

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
 
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
 
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}