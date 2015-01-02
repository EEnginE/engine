/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#version 330 core

layout(triangles) in;
layout(line_strip, max_vertices=8) out;

uniform mat4 uMVP;

in DataToGeom {
   vec4 normal;
   vec4 colorVertNormal;
   vec4 colorFaceNormal;
} vData[];

out vec4 toFragColor;

const float cNormalLength = 0.25;

void main() {
   int i;

   // The vertex normals
   for( i = 0; i < gl_in.length(); i++ ) {
      vec3 P = gl_in[i].gl_Position.xyz;
      vec3 N = vData[i].normal.xyz;

      gl_Position = uMVP * vec4(P, 1.0);
      toFragColor = vData[i].colorVertNormal;
      EmitVertex();

      gl_Position = uMVP * vec4(P + N * cNormalLength, 1.0);
      toFragColor = vData[i].colorVertNormal;
      EmitVertex();

      EndPrimitive();
   }

   vec3 Vec1 = gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz;
   vec3 Vec2 = gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz;

   vec3 FaceNormal = normalize( cross( Vec2, Vec1 ) );

   vec3 Center = ( gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz ) / 3;

   gl_Position = uMVP * vec4( Center , 1.0 );
   toFragColor = vData[0].colorFaceNormal;
   EmitVertex();

   gl_Position = uMVP * vec4( Center + cNormalLength * FaceNormal , 1.0 );
   toFragColor = vData[0].colorFaceNormal;
   EmitVertex();

   EndPrimitive();
}
