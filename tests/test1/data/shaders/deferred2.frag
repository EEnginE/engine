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

#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (input_attachment_index = 0, binding = 0) uniform subpassInput uPos;
layout (input_attachment_index = 1, binding = 1) uniform subpassInput uNormal;
layout (input_attachment_index = 2, binding = 2) uniform subpassInput uColor;

layout (location = 0) out vec4 outFragColor;

void main()
{
   vec4 lPos        = subpassLoad( uPos );
   vec4 lTempNormal = subpassLoad( uNormal );
   vec4 lTempColor  = subpassLoad( uColor );

   if( lPos == vec4( 0.0f, 0.0f, 0.0f, 0.0f ) ) {
      lPos = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
   } else {
      lPos = vec4( lPos.xyz, 1.0f );
   }

   outFragColor = lPos;
}
