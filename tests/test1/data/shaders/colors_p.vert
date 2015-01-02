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

layout (location = 0) in vec4 position;

// Just for testing the rLinker

struct someStuff {
  vec4 someVec;
  vec4 someMat;
};

uniform someOtherStuff {
  uniform vec4 someOtherVec;
  mat4 someOtherMat;
};

uniform mat4 aUniformValue;

uniform someStuff fancyStuff;
uniform someStuff notSoFancyStuff;
uniform someStuff Stuff[2];

void main()
{
   vec4 someSpecialStuff = someOtherVec * someOtherMat * Stuff[0].someVec * Stuff[1].someVec * Stuff[0].someMat * Stuff[1].someMat;
   vec4 etcStuff = notSoFancyStuff.someMat * notSoFancyStuff.someVec;
   vec4 anOtherVec = fancyStuff.someMat * fancyStuff.someVec;
	gl_Position = aUniformValue * position * etcStuff * someSpecialStuff * anOtherVec;
}
