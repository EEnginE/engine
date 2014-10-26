#version 120

attribute vec3 Position;

uniform mat4 gWorld;

void main()
{
    gl_Position = gWorld * vec4(Position, 1.0);
    gl_FrontColor = vec4(clamp(Position, 0.0, 1.0), 1.0);
}
