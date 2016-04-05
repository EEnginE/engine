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
#include "rPipeline.hpp"

namespace e_engine {

rSimpleMesh::rSimpleMesh( rMatrixSceneBase<float> *_scene, std::string _name )
    : rMatrixObjectBase( _scene ),
      rObjectBase( _name ),
      vIndex( _scene->getWorldPtr()->getInitPtr() ),
      vVertex( _scene->getWorldPtr()->getInitPtr() ) {}


/*!
 * \brief records the command buffer
 * \param _buf The command buffer to record
 * \vkIntern
 */
void rSimpleMesh::record( VkCommandBuffer _buf ) {
   VkDeviceSize lOffsets[] = {0};
   VkBuffer lVertex        = vVertex.getBuffer();

   vPipeline->cmdBindPipeline( _buf, VK_PIPELINE_BIND_POINT_GRAPHICS );
   vkCmdBindVertexBuffers( _buf, vPipeline->getVertexBindPoint(), 1, &lVertex, &lOffsets[0] );
   vkCmdBindIndexBuffer( _buf, vIndex.getBuffer(), 0, VK_INDEX_TYPE_UINT32 );
   vkCmdDrawIndexed( _buf, vIndex.getSize(), 1, 0, 0, 1 );
}

/*!
 * \brief Inits the object (partialy)
 * \note This function SHOULD NOT be called directly! Use the functions in rScene instead!
 */
std::vector<rBuffer *> rSimpleMesh::setData_IMPL( VkCommandBuffer _buf,
                                                  std::vector<uint32_t> const &_index,
                                                  std::vector<float> const &_pos,
                                                  std::vector<float> const &_norm,
                                                  std::vector<float> const & ) {
   iLOG( "Initializing simple mesh object ", vName_str );

   std::vector<float> lTemp;
   setupVertexData_PN( _pos, _norm, lTemp );

   vIndex.cmdInit( _index, _buf, VK_BUFFER_USAGE_INDEX_BUFFER_BIT );
   vVertex.cmdInit( lTemp, _buf, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT );

   return {&vIndex, &vVertex};
}

void rSimpleMesh::signalRenderReset() {
   if ( !vPipeline ) {
      eLOG( "Pipeline not setup!" );
      return;
   }

   vShader      = vPipeline->getShader();
   vVertUniform = vShader->getUniformBuffer( VK_SHADER_STAGE_VERTEX_BIT );

   if ( !vVertUniform ) {
      wLOG( "No uniform buffers in shader" );
      return;
   }

   vHasMVPMatrix = false;

   for ( auto const &i : vVertUniform->vars ) {
      if ( i.guessedRole == rShaderBase::MODEL_VIEW_PROJECTION_MATRIX ) {
         vHasMVPMatrix = true;
         vMatrixMVPVar = i;
      }
   }
}

void rSimpleMesh::updateUniforms() {
   if ( !vPipeline || !vShader ) {
      eLOG( "Pipeline / shader not setup!" );
      return;
   }

   if ( vHasMVPMatrix )
      vShader->updateUniform( vMatrixMVPVar, getModelViewProjectionMatrix()->getMatrix() );
}

bool rSimpleMesh::checkIsCompatible( rPipeline *_pipe ) {
   return _pipe->checkInputCompatible( {{3, sizeof( float )}, {3, sizeof( float )}} );
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
