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

#ifndef CMDANDINIT_HPP
#define CMDANDINIT_HPP

#include <engine.hpp>
#include <vector>
#include <string>


class cmdANDinit {
 private:
   std::vector<std::string> args;
   std::string argv0;
   std::string dataRoot;
   std::string meshToRender = "meshS";
   std::string vShader = "phong";
   std::string vNormalShader = "normals";

   e_engine::uJSON_data vData_JSON;

   bool vCanUseColor;
   bool vRenderNormals = false;

   GLfloat vNearZ = 0.1f;
   GLfloat vFarZ = 100.0f;

   cmdANDinit() {}

   void postInit();
   void preInit();
   void usage();

 public:
   cmdANDinit( int argc, char *argv[] );

   std::string getMesh() const { return dataRoot + "obj/" + meshToRender + ".obj"; }
   std::string getShader() const { return dataRoot + "shaders/" + vShader; }
   std::string getNormalShader() const { return dataRoot + "shaders/" + vNormalShader; }

   GLfloat getNearZ() const { return vNearZ; }
   GLfloat getFarZ() const { return vFarZ; }

   bool getRenderNormals() const { return vRenderNormals; }

   bool parseArgsAndInit();
};

#endif // CMDANDINIT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
