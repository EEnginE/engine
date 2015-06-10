/*!
 * \file rLoader_glTF.hpp
 * \brief \b Classes: \a rLoader_glTF
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

#ifndef R_LOADER_GLTF_HPP
#define R_LOADER_GLTF_HPP

#include "defines.hpp"

#include "rLoaderBase.hpp"
#include "rLoader_glTF_structs.hpp"
#include <string>

namespace e_engine {

class RENDER_API rLoader_glTF final : public internal::rLoaderBase<GLfloat, GLushort>,
                                      public glTF::rLoader_glTF_structs {

   typedef std::unordered_map<std::string, size_t> td_MAP;

 private:
   std::vector<accessor> vAccessors;
   std::vector<buffer> vBuffers;
   std::vector<bufferView> vBufferViews;
   std::vector<mesh> vMeshs;
   std::vector<material> vMaterials;

   td_MAP vAccessorMap;
   td_MAP vBufferMap;
   td_MAP vBufferViewMap;
   td_MAP vMeshMap;
   td_MAP vMaterialMap;

   bool load_IMPL();

   bool getMapElement( ELEMENTS &_el, bool _isSection = true );
   bool getMapElementETC( ELEMENTS &_el );
   bool skipSection();

   template <class T>
   size_t getItem( std::vector<T> &_vec, td_MAP &_map, std::string _id );

   bool sectionAccessors();
   bool sectionBufferViews();
   bool sectionBuffers();
   bool sectionMeshs();
   bool sectionMeshsPrimitives( size_t _id );

   bool interprete();

   bool skipSectionWarning( unsigned int _s );
   bool selfTestFailed( std::string _type, std::string _name );
   bool wrongKeyWordError();

   std::string vMapElStr;

 public:
   rLoader_glTF() {}
   rLoader_glTF( std::string _file ) : rLoaderBase( _file ) {}
};

template <class T>
size_t rLoader_glTF::getItem( std::vector<T> &_vec, td_MAP &_map, std::string _id ) {
   auto lIter = _map.find( _id );

   if ( lIter == _map.end() ) {
      _vec.emplace_back();
      _map[_id] = _vec.size() - 1;
      _vec.back().name = _id;
      return _vec.size() - 1;
   } else { return lIter->second; }
}
}

#endif // R_LOADER_GLTF_HPP
