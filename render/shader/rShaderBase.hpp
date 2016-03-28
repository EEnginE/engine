/*!
 * \file rShaderBase.hpp
 * \brief \b Classes: \a rShaderBase
 */
/*
 * Copyright (C) 2016 EEnginE project
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
 *
 */

#pragma once

#include "defines.hpp"
#include <vulkan.h>
#include <vector>
#include <string>

class rShaderBase {
public:
   typedef struct InOut {
      std::string type;
      std::string name;
      uint32_t location;
      uint32_t arraySize;
   } InOut;

   typedef struct Uniform {
      std::string name;
      uint32_t binding;

      std::vector<InOut> vars;
   } Uniform;

   typedef struct ShaderInfo {
      std::vector<InOut> input;
      std::vector<InOut> output;
      std::vector<Uniform> uniforms;
   } ShaderInfo;

   virtual bool has_vert() const = 0;
   virtual bool has_tesc() const = 0;
   virtual bool has_tese() const = 0;
   virtual bool has_geom() const = 0;
   virtual bool has_frag() const = 0;
   virtual bool has_comp() const = 0;

   virtual ShaderInfo getInfo_vert() const = 0;
   virtual ShaderInfo getInfo_tesc() const = 0;
   virtual ShaderInfo getInfo_tese() const = 0;
   virtual ShaderInfo getInfo_geom() const = 0;
   virtual ShaderInfo getInfo_frag() const = 0;
   virtual ShaderInfo getInfo_comp() const = 0;

   virtual std::vector<unsigned char> getRawData_vert() const = 0;
   virtual std::vector<unsigned char> getRawData_tesc() const = 0;
   virtual std::vector<unsigned char> getRawData_tese() const = 0;
   virtual std::vector<unsigned char> getRawData_geom() const = 0;
   virtual std::vector<unsigned char> getRawData_frag() const = 0;
   virtual std::vector<unsigned char> getRawData_comp() const = 0;
};
