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

const int MAX_LIGHTS = 3;

uniform mat4 uModelView;

out vec4 oFinalColor;

smooth in vec3 vModelView;
smooth in vec3 vNormals;

smooth in vec3 vAmbientDiffuseMaterial;

// Light stuff

uniform int uNumLights;

uniform struct Light {
   int  type;

   vec3 ambient;
   vec3 color;
   vec3 position; // Also direction for directional Light
   vec3 attenuation;
} uLights[MAX_LIGHTS];

const vec3 cSpecularMaterial = vec3( 0.9, 0.9, 0.9 );
const float cShininess       = 30.0;


vec3 DirectionalLight( int i ) {
   // Diffuse Light
   float lIntensity = max( 0, dot( vNormals, -uLights[i].position ) );
   vec3 lResult     = vec3( 0 );

   if( lIntensity > 0 ) {
      lResult          = vAmbientDiffuseMaterial * uLights[i].color * lIntensity;

      // Specular Light
      vec3 lReflection = normalize( reflect( uLights[i].position, vNormals) );
      lIntensity       = max( 0.0, dot( -normalize( vModelView ), lReflection ) );

      lResult         += cSpecularMaterial * uLights[i].color * pow( lIntensity, cShininess );
   }

   return lResult;
}

vec3 PointLight( int i ) {
   // Diffuse Light
   vec3 lDirection    = uLights[i].position - vModelView;
   float lDistance    = length( lDirection );
   lDirection         = normalize( lDirection );

   float lIntensity   = max( 0, dot( vNormals, lDirection ) );

   vec3 lResult = vec3( 0 );

   if( lIntensity > 0 ) {
      lResult          = vAmbientDiffuseMaterial * uLights[i].color * lIntensity;

      // Specular Light
      vec3 lReflection = normalize( reflect( -lDirection, vNormals) );
      lIntensity       = max( 0.0, dot( -normalize( vModelView ), lReflection ) );

      lResult         += cSpecularMaterial * uLights[i].color * pow( lIntensity, cShininess );

      float lAttenuation = uLights[i].attenuation.x +
                           uLights[i].attenuation.y * lDistance +
                           uLights[i].attenuation.z * lDistance * lDistance +1;

      return lResult / lAttenuation;
   }

   return lResult;
}

void main(void) {
   vec3 lReflection;
   float lIntensity;

   vec3 lLight         = vec3( 0 );
   vec3 lAmbientLight  = vec3( 0 );

   for( int i = 0; i < uNumLights; ++i ) {
      lAmbientLight  += vAmbientDiffuseMaterial * uLights[i].ambient;

      if( uLights[i].type == 0 ) {lLight += DirectionalLight( i );}
      if( uLights[i].type == 1 ) {
         lLight += PointLight( i );
      }
   }

   oFinalColor = vec4( lAmbientLight + lLight, 1 );
}
