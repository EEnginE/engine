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

#version 120

attribute vec3 iVertex;

uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * vec4(iVertex, 1.0);
    gl_FrontColor = vec4(clamp(iVertex, 0.0, 1.0), 1.0);
}
