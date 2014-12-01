#version 120

attribute vec3 iVertex;

uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * vec4(iVertex, 1.0);
    gl_FrontColor = vec4(clamp(iVertex, 0.0, 1.0), 1.0);
}
