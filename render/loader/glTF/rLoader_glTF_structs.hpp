/*!
 * \file rLoader_glTF_structs.hpp
 * \brief \b Classes: \a rLoader_glTF_structs
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
#include "rLoader_glTF_map.hpp"
#include <string>
#include <vector>

#if D_LOG_GLTF
#define GLTF_STRUCTS_PRINT void print( rLoader_glTF_structs *_parent ) const;
#else
#define GLTF_STRUCTS_PRINT
#endif

namespace e_engine {
namespace glTF {

class UTILS_API rLoader_glTF_structs : public rLoader_glTF_map {
 private:
#if D_LOG_GLTF
   std::string getStringFromElement( ELEMENTS _el );
#endif

 protected:
   struct base {
      std::string id;
      std::string name;
   };

   struct accessor : base {
      size_t bufferView      = static_cast<size_t>( -1 ); //!< required
      int byteOffset         = -1;                        //!< required; min: 0
      int byteStride         = 0;                         //!< min: 0; max: 255
      ELEMENTS componentType = TYPE;                      //!< required
      int count              = -1;                        //!< required; min: 1
      ELEMENTS type          = TYPE;                      //!< required

      bool test() const;

      GLTF_STRUCTS_PRINT
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


   struct mesh : base {
      std::string userDefName;
      struct _primitives {
         struct _attributes {
            ELEMENTS type;                               //!< required
            size_t accessor = static_cast<size_t>( -1 ); //!< required
         };

         std::vector<_attributes> attributes;
         size_t indices     = static_cast<size_t>( -1 ); //!< required;
         size_t material    = static_cast<size_t>( -1 ); //!< required;
         ELEMENTS mode      = P_TRIANGLES;
      };

      std::vector<_primitives> primitives;

      bool test() const;

      GLTF_STRUCTS_PRINT
   };

   struct material : base {
      bool test() const;

      GLTF_STRUCTS_PRINT
   };

 public:
   virtual ~rLoader_glTF_structs();
};
}
}

#endif // R_LOADER_GLTF_STRUCTS_HPP
