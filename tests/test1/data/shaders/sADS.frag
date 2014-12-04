#version 330 core

in  vec3 toFragColor;
out vec4 oColor;

void main() {
   oColor = vec4( toFragColor, 1 );
}