#version 120

in vec3 Position;

uniform mat4 gWorld;

varying out vec4 Color;

void main()
{
    gl_Position = gWorld * vec4(Position, 1.0);
    Color = vec4(clamp(Position, 0.0, 1.0), 1.0);
}
