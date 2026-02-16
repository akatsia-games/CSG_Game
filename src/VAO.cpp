#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "GLWrappers.hpp"

#include "VAO.hpp"

#include <iostream>

VAO::VAO(std::vector<Vertex>& vertices, GLint vpos, GLint vcol)
{
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(vpos);
    glVertexAttribPointer(vpos, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, pos));

    glEnableVertexAttribArray(vcol);
    glVertexAttribPointer(vcol, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, col));
}

VAO::~VAO()
{
    glDeleteBuffers(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void VAO::draw()
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}