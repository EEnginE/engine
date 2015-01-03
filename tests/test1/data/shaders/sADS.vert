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

// Matrixes

uniform mat4 uModelView;
uniform mat4 uMVP;
uniform mat3 uNormal;

// Light stuff

uniform vec3 uAmbientColor;

struct Light {
   vec3 color;
   vec3 position;
};
uniform Light uLights;

in vec3 iVertex;
in vec3 iNormals;

out vec4 toFragColor;


const vec3 cSpecularMaterial = vec3( 0.9, 0.9, 0.9 );

void main() {
   vec3 lAmbientDiffuseMaterial = clamp(iVertex, 0.0, 1.0); // Make some colors...

   // Ambient Light
   vec3 lAmbientLight = lAmbientDiffuseMaterial * uAmbientColor;


   // Specular Light
   vec3  lNormalsEyeSpace  = uNormal * iNormals;   // Get the normals in world space
   vec3  lVertexWorldSpace = (uModelView * vec4( iVertex, 1.0 )).xyz;
   vec3  lLightDirection   = normalize( uLights.position - lVertexWorldSpace );

   vec3  lReflection       = normalize( reflect( -lLightDirection, lNormalsEyeSpace) );
   float spec              = max( 0.0, dot( -normalize(lVertexWorldSpace), lReflection ) );

   vec3  lSpecularLight    = cSpecularMaterial * pow( spec, 20.0 );


   // Diffuse Light
   float lDiffIntensity      = max( 0.0, dot( lNormalsEyeSpace, lLightDirection ) );
   vec3  lDiffuseLight       = lAmbientDiffuseMaterial * uLights.color * lDiffIntensity;

   toFragColor = vec4( lAmbientLight + lDiffuseLight + lSpecularLight, 1 );
   gl_Position = uMVP * vec4( iVertex, 1.0 );
}
