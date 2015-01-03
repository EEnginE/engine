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

uniform mat4 uModelView;

out vec4 oFinalColor;

smooth in vec3 vPosition;
smooth in vec3 vModelView;
smooth in vec3 vNormals;

smooth in vec3 vAmbientDiffuseMaterial;
smooth in vec3 vAmbientLight;
smooth in vec3 vLightDirection;

// Light stuff

struct Light {
   vec3 color;
   vec3 position;
};
uniform Light uLights;

const vec3 cSpecularMaterial = vec3( 0.9, 0.9, 0.9 );
const float cShininess       = 30.0;

void main(void) {

   // Specular Light
   vec3  lReflection       = normalize( reflect( -vLightDirection, vNormals) );
   float spec              = max( 0.0, dot( -normalize(vModelView), lReflection ) );

   vec3  lSpecularLight    = cSpecularMaterial * pow( spec, cShininess );

   // Diffuse Light
   float lDiffIntensity      = max( 0, dot( vNormals, vLightDirection ) );
   vec3  lDiffuseLight       = vAmbientDiffuseMaterial * uLights.color * lDiffIntensity;

   oFinalColor = vec4( vAmbientLight + lDiffuseLight + lSpecularLight, 1 );
}
