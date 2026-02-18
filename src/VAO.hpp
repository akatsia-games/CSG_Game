#pragma 

#include<vector>

class GLVertex;

class VAO{
public:
    VAO(std::vector<GLVertex>& vertices, GLint vpos, GLint vcol);
    ~VAO();

    void draw();
private:
    GLuint vao;
    GLuint vbo;
    GLuint count;
};