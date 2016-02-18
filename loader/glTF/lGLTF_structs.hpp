/*!
 * \file lGLTF_structs.hpp
 * \brief \b Classes: \a lGLTF_structs
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

#ifndef R_LOADER_GLTF_STRUCTS_HPP
#define R_LOADER_GLTF_STRUCTS_HPP

#include "defines.hpp"
#include "lGLTF_map.hpp"
#include <string>
#include <vector>

#if D_LOG_GLTF
#define GLTF_STRUCTS_PRINT void print( lGLTF_structs *_parent ) const;
#define GLTF_STRUCTS_PRINT2 void print() const;
#else
#define GLTF_STRUCTS_PRINT
#define GLTF_STRUCTS_PRINT2
#endif

namespace e_engine {
namespace glTF {

class UTILS_API lGLTF_structs : public lGLTF_map {
 private:
#if D_LOG_GLTF
   std::string getStringFromElement( ELEMENTS _el );
#endif

 protected:
   struct base {
      std::string id;
      std::string name;
   };

   struct value : base {
      enum TYPE { NUM, BOOL, STR, A_NUM, A_BOOL, A_STR, __NOT_SET__ };
      TYPE type = __NOT_SET__;

      std::vector<bool> valBool;
      std::vector<float> valNum;
      std::vector<std::string> valStr;

      GLTF_STRUCTS_PRINT2
   };

   struct accessor : base {
      size_t bufferView      = static_cast<size_t>( -1 ); //!< required
      int byteOffset         = -1;                        //!< required; min: 0
      int byteStride         = 0;                         //!< min: 0; max: 255
      ELEMENTS componentType = TYPE;                      //!< required
      int count              = -1;                        //!< required; min: 1
      ELEMENTS type          = TYPE;                      //!< required

      std::vector<float> max;
      std::vector<float> min;

      bool test() const;

      GLTF_STRUCTS_PRINT
   };

   struct asset {
      std::string copyright   = "";
      std::string generator   = "";
      bool premultipliedAlpha = false;
      std::string version; //!< required

      struct profileStruct {
         std::string api     = "WebGL";
         std::string version = "1.0.3";
      } profile;

      bool test() const;

      GLTF_STRUCTS_PRINT2
   };

   struct buffer : base {
      std::string uri;                  //!< required
      int byteLength = 0;               //!< min: 0
      ELEMENTS type  = TG_ARRAY_BUFFER; //!< required

      bool test() const;

      GLTF_STRUCTS_PRINT
   };

   struct bufferView : base {
      size_t buffer   = static_cast<size_t>( -1 ); //!< required
      int byteOffset  = -1;                        //!< required; min: 0
      int byteLength  = 0;                         //!< min: 0
      ELEMENTS target = TYPE;                      //!< required

      bool test() const;

      GLTF_STRUCTS_PRINT
   };

   struct camera : base {};

   struct image : base {
      std::string uri; //!< required

      bool test() const;

      GLTF_STRUCTS_PRINT2
   };

   struct material : base {
      size_t technique = static_cast<size_t>( -1 );

      std::vector<value> values;

      bool test() const;

      GLTF_STRUCTS_PRINT2
   };


   struct mesh : base {
      std::string userDefName;
      struct _primitives {
         struct _attributes {
            ELEMENTS type;                               //!< required
            size_t accessor = static_cast<size_t>( -1 ); //!< required
         };

         std::vector<_attributes> attributes;
         size_t indices  = static_cast<size_t>( -1 ); //!< required;
         size_t material = static_cast<size_t>( -1 ); //!< required;
         ELEMENTS mode   = P_TRIANGLES;
      };

      std::vector<_primitives> primitives;

      bool test() const;

      GLTF_STRUCTS_PRINT
   };

   struct node : base {
      size_t camera = static_cast<size_t>( -1 );
      std::vector<size_t> children;
      std::vector<size_t> skeletons;
      size_t skin = static_cast<size_t>( -1 );
      std::string jointName;
      std::vector<float> matrix;
      std::vector<size_t> meshes;
      std::vector<float> rotation;
      std::vector<float> scale;
      std::vector<float> translation;

      bool test() const;

      GLTF_STRUCTS_PRINT2
   };

   struct program : base {
      std::vector<std::string> attributes;
      size_t fragmentShader = static_cast<size_t>( -1 ); //!< required;
      size_t vertexShader   = static_cast<size_t>( -1 ); //!< required;

      bool test() const;

      GLTF_STRUCTS_PRINT2
   };

   struct scene : base {
      std::vector<size_t> nodes;

      bool test() const { return true; }

      GLTF_STRUCTS_PRINT2
   };

   struct shader : base {
      std::string uri; //!< required;
      ELEMENTS type;   //!< required;

      bool test() const;

      GLTF_STRUCTS_PRINT
   };

   struct skin : base {
      std::vector<float> bindShapeMatrix = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
      size_t inverseBindMatrices         = static_cast<size_t>( -1 ); //!< required;
      std::vector<size_t> jointNames;                                 //!< required;

      bool test() const;

      GLTF_STRUCTS_PRINT2
   };

   struct technique : base {
      typedef std::unordered_map<std::string, size_t> td_MAP;

      struct parameter : base {
         int count = 0;
         size_t node;
         ELEMENTS type; //!< required;
         ELEMENTS semantic;
         value lValue;

         GLTF_STRUCTS_PRINT
      };

      struct attribute {
         std::string id;
         size_t parameter;

         GLTF_STRUCTS_PRINT2
      };

      struct _states {
         struct {
            std::vector<float> blendColor               = {0, 0, 0, 0};
            std::vector<ELEMENTS> blendEquationSeparate = {TECH_FUNC_ADD, TECH_FUNC_ADD};
            std::vector<int> blendFuncSeparate          = {1, 1, 0, 0};
            std::vector<bool> colorMask                 = {true, true, true, true};
            std::vector<ELEMENTS> cullFace              = {TECH_BACK};
            std::vector<ELEMENTS> depthFunc             = {TECH_LESS};
            std::vector<bool> depthMask                 = {true};
            std::vector<float> depthRange               = {0, 1};
            std::vector<ELEMENTS> frontFace             = {TECH_CCW};
            std::vector<float> lineWidth                = {1};
            std::vector<float> polygonOffset            = {0, 0};
            std::vector<float> scissor                  = {0, 0, 0, 0};
         } functions;

         std::vector<ELEMENTS> enable;

         GLTF_STRUCTS_PRINT
      } states;

      td_MAP parametersMap;

      std::vector<parameter> parameters;
      std::vector<attribute> attributes;
      std::vector<attribute> uniforms;

      size_t program = static_cast<size_t>( -1 ); //!< required;

      bool test() const;

      GLTF_STRUCTS_PRINT
   };


 public:
   virtual ~lGLTF_structs();
};
}
}

#endif // R_LOADER_GLTF_STRUCTS_HPP
