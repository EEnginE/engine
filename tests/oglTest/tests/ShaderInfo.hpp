/*!
 * \file ShaderInfo.hpp
 * \brief \b Classes: \a ShaderInfo
 *
 * Class for testing:
 * Tests if it is possible to query shader information
 *
 */
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

#ifndef SHADERINFO_HPP
#define SHADERINFO_HPP

#include <engine.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace e_engine;

class ShaderInfo {
 private:
   // Private stuff goes here

 public:
   ShaderInfo() {}

   const static string desc;

   void runTest( uJSON_data &_data, string _dataRoot );
};

#endif // SHADERINFO_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
