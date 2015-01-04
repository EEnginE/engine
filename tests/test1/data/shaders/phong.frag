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

const int MAX_LIGHTS = 2;

uniform mat4 uModelView;

out vec4 oFinalColor;

smooth in vec3 vPosition;
smooth in vec3 vModelView;
smooth in vec3 vNormals;

smooth in vec3 vAmbientDiffuseMaterial;
smooth in vec3 vAmbientLight;
smooth in vec3 vLightDirection[MAX_LIGHTS];

// Light stuff

uniform int uNumLights;

uniform struct Light {
   vec3 color;
   vec3 position;
} uLights[MAX_LIGHTS];

const vec3 cSpecularMaterial = vec3( 0.9, 0.9, 0.9 );
const float cShininess       = 30.0;


void main(void) {
   vec3 lReflection, lSpecularLight = vec3( 0 ), lDiffuseLight = vec3( 0 );
   float lIntensity;

   // Specular Light
   for( int i = 0; i < uNumLights; ++i ) {
      lReflection     = normalize( reflect( -vLightDirection[i], vNormals) );
      lIntensity      = max( 0.0, dot( -normalize(vModelView), lReflection ) );

      lSpecularLight += cSpecularMaterial * uLights[i].color * pow( lIntensity, cShininess );


      // Diffuse Light
      lIntensity      = max( 0, dot( vNormals, vLightDirection[i] ) );
      if( lReflection != vec3( 0, 0, 0 ) ) { // Strange theoretically unnecessary if
         lDiffuseLight  += vAmbientDiffuseMaterial * uLights[i].color * lIntensity;
      }
   }

   oFinalColor = vec4( vAmbientLight + lDiffuseLight + lSpecularLight, 1 );
}
