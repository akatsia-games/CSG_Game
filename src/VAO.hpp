#pragma 

#include<vector>

class Vertex;

class VAO{
public:
    VAO(std::vector<Vertex>& vertices, GLint vpos, GLint vcol);
    ~VAO();

    void draw();
private:
    GLuint vao;
    GLuint vbo;
};