/*!
 * \file rSimpleMesh.cpp
 * \brief \b Classes: \a rSimpleMesh
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

#include "rSimpleMesh.hpp"
#include "rWorld.hpp"
#include "uLog.hpp"

namespace e_engine {

rSimpleMesh::rSimpleMesh( rMatrixSceneBase<float> *_scene, std::string _name )
    : rMatrixObjectBase( _scene ),
      rObjectBase( _name ),
      vIndex( _scene->getWorldPtr()->getInitPtr() ),
      vVertex( _scene->getWorldPtr()->getInitPtr() ) {}

/*!
 * \brief Inits the object (partialy)
 * \note This function SHOULD NOT be called directly! Use the functions in rScene instead!
 */
bool rSimpleMesh::setData( VkCommandBuffer _buf,
                           std::vector<uint32_t> const &_index,
                           std::vector<float> const &_pos,
                           std::vector<float> const &_norm,
                           std::vector<float> const & ) {
   if ( vIsLoaded_B ) {
      eLOG( "Data already loaded! Object ", vName_str );
      return false;
   }

   std::vector<float> lTemp;

   iLOG( "Initializing simple mesh object ", vName_str );

   if ( _pos.size() != _norm.size() || ( _pos.size() % 3 ) != 0 ) {
      eLOG( "Invalid data! Object ", vName_str );
      return false;
   }

   lTemp.resize( _pos.size() * 2 );
   for ( uint32_t i = 0, counter = 0; i < _pos.size(); i += 3, counter++ ) {
      lTemp[6 * counter + 0] = _pos[i + 0];
      lTemp[6 * counter + 1] = _pos[i + 1];
      lTemp[6 * counter + 2] = _pos[i + 2];
      lTemp[6 * counter + 3] = _norm[i + 0];
      lTemp[6 * counter + 4] = _norm[i + 1];
      lTemp[6 * counter + 5] = _norm[i + 2];
   }

   bool lRes = true;
   lRes      = lRes && vIndex.cmdInit( _index, _buf, VK_BUFFER_USAGE_INDEX_BUFFER_BIT );
   lRes      = lRes && vVertex.cmdInit( lTemp, _buf, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT );
   return lRes;
}

/*!
 * \brief Inits the object (partialy)
 * \note This function SHOULD NOT be called directly! Use the functions in rScene instead!
 */
bool rSimpleMesh::finishData() {
   if ( vIsLoaded_B ) {
      eLOG( "Data already loaded! Object ", vName_str );
      return false;
   }

   bool lRes = true;
   lRes      = lRes && vIndex.doneCopying();
   lRes      = lRes && vVertex.doneCopying();

   if ( lRes )
      vIsLoaded_B = true;

   return lRes;
}

uint32_t rSimpleMesh::getMatrix( rMat4f **_mat, rObjectBase::MATRIX_TYPES _type ) {
   switch ( _type ) {
      case SCALE: *_mat                 = getScaleMatrix(); return 0;
      case ROTATION: *_mat              = getRotationMatrix(); return 0;
      case TRANSLATION: *_mat           = getTranslationMatrix(); return 0;
      case CAMERA_MATRIX: *_mat         = getViewProjectionMatrix(); return 0;
      case MODEL_MATRIX: *_mat          = getModelMatrix(); return 0;
      case VIEW_MATRIX: *_mat           = getViewMatrix(); return 0;
      case PROJECTION_MATRIX: *_mat     = getProjectionMatrix(); return 0;
      case MODEL_VIEW_MATRIX: *_mat     = getModelViewMatrix(); return 0;
      case MODEL_VIEW_PROJECTION: *_mat = getModelViewProjectionMatrix(); return 0;
      case NORMAL_MATRIX: break;
   }

   return INDEX_OUT_OF_RANGE;
}

uint32_t rSimpleMesh::getMatrix( rMat3f **_mat, rObjectBase::MATRIX_TYPES _type ) {
   switch ( _type ) {
      case NORMAL_MATRIX: *_mat = getNormalMatrix(); return 0;
      default: return INDEX_OUT_OF_RANGE;
   }
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
