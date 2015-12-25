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
      enum TYPE { NUM, BOOL, STR, A_NUM, A_BOOL, A_STR };
      TYPE type;

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

   struct technique : base {
      bool test() const;

      GLTF_STRUCTS_PRINT
   };


 public:
   virtual ~lGLTF_structs();
};
}
}

#endif // R_LOADER_GLTF_STRUCTS_HPP
