/*!
 * \file rLoader_3D_OBJ.hpp
 * \brief \b Classes: \a rLoader_3D_OBJ
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

#ifndef R_LOADER_3D_F_OBJ_H
#define R_LOADER_3D_F_OBJ_H

#include "defines.hpp"

#include "lLoaderBase.hpp"
#include <string>
#include <vector>

#include <GL/glew.h>

namespace e_engine {

class LOADER_API rLoader_3D_f_OBJ : public internal::lLoaderBase<GLfloat, GLushort> {
 private:
   std::string vName;

   bool load_IMPL();

 public:
   rLoader_3D_f_OBJ() {}
   rLoader_3D_f_OBJ( std::string _file ) : lLoaderBase( _file ) {}
   virtual ~rLoader_3D_f_OBJ() {}
};
}

#endif // R_LOADER_3D_F_OBJ_H

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
