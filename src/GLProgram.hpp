#pragma once

class GLProgram{
public:
    GLProgram(const char* const vertexShader, const char* const fragmentShader);
    
    GLint operator[](const char* const uniformId);

private:

};