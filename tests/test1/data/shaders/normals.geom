#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices=6) out;

uniform mat4 uMVP;

in DataToGeom {
   vec4 normal;
   vec4 color;
} vData[];

out vec4 toFragColor;

const float cNormalLength = 0.25;

void main() {
   int i;
   for( i = 0; i < gl_in.length(); i++ ) {
      vec3 P = gl_in[i].gl_Position.xyz;
      vec3 N = vData[i].normal.xyz;

      gl_Position = uMVP * vec4(P, 1.0);
      toFragColor = vData[i].color;
      EmitVertex();

      gl_Position = uMVP * vec4(P + N * cNormalLength, 1.0);
      toFragColor = vData[i].color;
      EmitVertex();

      EndPrimitive();
   }
}
