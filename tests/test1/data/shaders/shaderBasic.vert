#version 330

in vec3 vVertex;

uniform mat4 vMatrix;

out vec4 vColorVarying;

void main(void) {
   vColorVarying = vec4( clamp(vVertex, 0.0, 1.0), 1.0 );
   gl_Position = vMatrix * vec4( vVertex.xyz, 1.0 );
}