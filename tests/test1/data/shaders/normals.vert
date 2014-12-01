#version 330 core

in vec3 iVertex;
in vec3 iNormals;

out DataToGeom {
   vec4 normal;
   vec4 color;
} vData;

void main() {
   gl_Position  = vec4( iVertex, 1.0 );
   vData.normal = vec4( iNormals, 1.0 );
   vData.color  = vec4( 1.0 , 1.0, 1.0, 1.0 );
}
