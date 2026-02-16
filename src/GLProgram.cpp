#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <fstream>

#include "GLProgram.hpp"

#include <iostream>


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

GLProgram::GLProgram(const char* const vertexShader, const char* const fragmentShader)
{
    readShader(vertex_shader, GL_VERTEX_SHADER, vertexShader);
    checkShaderErrors(vertex_shader);

    readShader(fragment_shader, GL_FRAGMENT_SHADER, fragmentShader);
    checkShaderErrors(fragment_shader);
 
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    checkProgramErrors();
}



GLint GLProgram::uniform(const GLchar* uniformId)const
{
    return glGetUniformLocation(program, uniformId);
}

GLuint GLProgram::pointer()
{
    return program;
}

void GLProgram::use()
{
    glUseProgram(program);
}


void GLProgram::readShader(GLuint& shader, GLuint shader_type, const char* const shaderLocation)
{
    char buffer[1024];
    buffer[1023] = '\0';
    std::ifstream shaderFile(shaderLocation);

    std::string shaderText;

    std::streamsize extracted = 0;
    do
    {
        shaderFile.getline(buffer, 1023, '\0');
        extracted = shaderFile.gcount();

        shaderText+=buffer;
    } while ((extracted < 1023) && buffer[1022]!='\0');
    
    const char* const shaderText_c = shaderText.c_str();

    shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &(shaderText_c), NULL);
    glCompileShader(shader);
}

void GLProgram::checkProgramErrors(){

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if(success == GL_TRUE) return;

    GLint errlen;
    glGetProgramiv(program,GL_INFO_LOG_LENGTH,&errlen);

    char* error_log = new char[errlen+1];
    GLint errorRead;

    glGetProgramInfoLog(program,errlen,&errorRead,error_log);

    std::cerr<<"Program errors:"<<errorRead<<"\n"<<error_log<<std::endl;
}

void GLProgram::checkShaderErrors(GLuint& shader){

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(success == GL_TRUE) return;

    GLint errlen;
    glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&errlen);

    char* error_log = new char[errlen+1];
    GLint errorRead;

    glGetShaderInfoLog(shader,errlen,&errorRead,error_log);

    std::cerr<<"Shader errors at ["<<shader<<"]:"<<errorRead<<"\n"<<error_log<<std::endl;
}