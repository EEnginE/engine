/*!
 * \file lGLTF.hpp
 * \brief \b Classes: \a lGLTF
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

#pragma once

#include "defines.hpp"

#include "lLoaderBase.hpp"
#include "lGLTF_structs.hpp"
#include <string>

#if D_LOG_GLTF
#define dLOG_glTF( ... ) dLOG( __VA_ARGS__ )
#else
#define dLOG_glTF( ... )
#endif

namespace e_engine {

class LOADER_API lGLTF final : public internal::lLoaderBase<float, unsigned short>,
                               public glTF::lGLTF_structs {

   typedef std::unordered_map<std::string, size_t> td_MAP;

 private:
   std::string vScene;

   std::vector<accessor> vAccessors;
   std::vector<buffer> vBuffers;
   std::vector<bufferView> vBufferViews;
   std::vector<camera> vCameras;
   std::vector<image> vImages;
   std::vector<material> vMaterials;
   std::vector<mesh> vMeshs;
   std::vector<node> vNodes;
   std::vector<program> vPrograms;
   std::vector<shader> vShaders;
   std::vector<scene> vScenes;
   std::vector<skin> vSkins;
   std::vector<technique> vTechniques;

   asset vAsset;

   td_MAP vAccessorMap;
   td_MAP vBufferMap;
   td_MAP vBufferViewMap;
   td_MAP vCamerasMap;
   td_MAP vImageMap;
   td_MAP vMaterialMap;
   td_MAP vMeshsMap;
   td_MAP vNodesMap;
   td_MAP vProgramsMap;
   td_MAP vShadersMap;
   td_MAP vScenesMap;
   td_MAP vSkinsMap;
   td_MAP vTechniqueMap;

   bool load_IMPL();

   bool getMapElement( ELEMENTS &_el, bool _isSection = true );
   bool getMapElementETC( ELEMENTS &_el );
   bool getBoolean( bool &_value );
   bool getArray( std::vector<int> &_array );
   bool getArray( std::vector<float> &_array );
   bool getArray( std::vector<bool> &_array );
   bool getArray( std::vector<std::string> &_array );
   bool getArray( std::vector<ELEMENTS> &_array );
   bool getValue( value &_val );
   bool skipSection();

   template <class T>
   size_t getItem( std::vector<T> &_vec, td_MAP &_map, std::string _id );

   bool sectionAccessors();
   bool sectionAsset();
   bool sectionBufferViews();
   bool sectionBuffers();
   bool sectionImages();
   bool sectionMaterials();
   bool sectionMeshs();
   bool sectionMeshsPrimitives( size_t _id );
   bool sectionNodes();
   bool sectionPrograms();
   bool sectionScene();
   bool sectionScenes();
   bool sectionShaders();
   bool sectionSkins();
   bool sectionTechniques();
   bool sectionTechniquesParameters( size_t _id );
   bool sectionTechniquesAttributes( size_t _id, std::vector<technique::attribute> &_attrs );
   bool sectionTechniquesStates( size_t _id );

   bool interprete();

   bool skipSectionWarning( unsigned int _s );
   bool selfTestFailed( std::string _type, std::string _name );
   bool wrongKeyWordError();

   std::string vMapElStr;

 public:
   lGLTF() {}
   lGLTF( std::string _file ) : lLoaderBase( _file ) {}
};

template <class T>
size_t lGLTF::getItem( std::vector<T> &_vec, td_MAP &_map, std::string _id ) {
   auto lIter = _map.find( _id );

   if ( lIter == _map.end() ) {
      _vec.emplace_back();
      _map[_id] = _vec.size() - 1;
      _vec.back().id = _id;
      return _vec.size() - 1;
   } else {
      return lIter->second;
   }
}
}
