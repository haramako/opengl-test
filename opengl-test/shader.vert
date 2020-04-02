#version 120

//
// shader.vert
//

uniform mat4 projectionMatrix;

void main(void)
{
    gl_Position = projectionMatrix * gl_Vertex;
    gl_FrontColor = gl_Color;
}
