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

#version 330

in vec3 iVertex;
in vec3 iNormals;

uniform mat4 uModelView;
uniform mat4 uMVP;
uniform mat3 uNormal;

smooth out vec3 vPosition;
smooth out vec3 vModelView;
smooth out vec3 vNormals;

// Collors...

smooth out vec3 vAmbientDiffuseMaterial; // Make some colors...
smooth out vec3 vAmbientLight;
smooth out vec3 vLightDirection;

uniform vec3 uAmbientColor;

struct Light {
   vec3 color;
   vec3 position;
};
uniform Light uLights;

void main(void) {
   vAmbientDiffuseMaterial = clamp(iVertex, 0.0, 1.0);

   vAmbientLight = vAmbientDiffuseMaterial * uAmbientColor;

   gl_Position = uMVP * vec4( iVertex.xyz, 1.0 );

   vPosition  = gl_Position.xyz;
   vNormals   =   uNormal    * iNormals;
   vModelView = ( uModelView * vec4( iVertex , 1 )).xyz;

   vLightDirection = normalize( uLights.position - vModelView );
}
