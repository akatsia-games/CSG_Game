#version 330
uniform mat4 MVP;
uniform vec3 oPos;
in vec3 vCol;
in vec3 vPos;
out vec3 color;
void main()
{
    gl_Position = MVP * vec4(vPos + oPos, 1.0);
    color = vCol;
}