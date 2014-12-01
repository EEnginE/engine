#version 330

in vec3 iVertex;

uniform mat4 uMVP;

out vec4 vColorVarying;

void main(void) {
   vColorVarying = vec4( clamp(iVertex, 0.0, 1.0), 1.0 );
   gl_Position = uMVP * vec4( iVertex.xyz, 1.0 );
}