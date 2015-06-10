/*!
 * \file uExtensions.hpp
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

#ifndef U_EXTENSIONS_HPP
#define U_EXTENSIONS_HPP

#include "defines.hpp"
#include <string>
#include <vector>

namespace e_engine {

enum EXTENSIONS { ID_ARB_program_interface_query = 0, __EXTENSIONS_END__ };

enum OPENGL_VERSIONS {
   OGL_VERSION_NONE = -1,
   OGL_VERSION_1_2 = 0,
   OGL_VERSION_1_3,
   OGL_VERSION_1_4,
   OGL_VERSION_1_5,

   OGL_VERSION_2_0,
   OGL_VERSION_2_1,

   OGL_VERSION_3_0,
   OGL_VERSION_3_1,
   OGL_VERSION_3_2,
   OGL_VERSION_3_3,

   OGL_VERSION_4_0,
   OGL_VERSION_4_1,
   OGL_VERSION_4_2,
   OGL_VERSION_4_3,
   OGL_VERSION_4_4,
   OGL_VERSION_4_5,
};



struct uExtensionData {
   EXTENSIONS id;
   std::string extStr;
   bool supported;
};

class UTILS_API uExtensions {
 private:
   uExtensionData *vOpenGLExtList;
   OPENGL_VERSIONS vVersion;

 public:
   uExtensions();
   virtual ~uExtensions();

   OPENGL_VERSIONS queryAll();
   bool isSupported( std::string _ext );
   bool isSupported( EXTENSIONS _id ) { return vOpenGLExtList[_id].supported; }

   OPENGL_VERSIONS getOpenGLVersion() { return vVersion; }
};
}

#endif // UEXTENSIONS_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
