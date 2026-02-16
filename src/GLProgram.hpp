#pragma once

class GLProgram{
public:
    GLProgram(const char* const vertexShader, const char* const fragmentShader);
    
    GLint uniform(const GLchar* uniformId) const;

    GLuint pointer();

    void use(); 
private:
    void readShader(GLuint& shader, GLuint shader_type, const char* const shaderLocation);

    void checkProgramErrors();
    void checkShaderErrors(GLuint& shader);

    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
};