#version 140

//
// shader.vert
//

uniform mat4 projectionMatrix;

in vec3 position;
in vec4 color;

void main(void)
{
    gl_Position = projectionMatrix * vec4(position, 1.0);
    gl_FrontColor = color;
}
