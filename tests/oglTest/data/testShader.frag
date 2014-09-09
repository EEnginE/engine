#version 330

in vec4 Color;

layout (location = 2) out vec4 FragColor;

void main()
{
    FragColor = Color;
}

