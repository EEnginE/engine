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
#include "rPipeline.hpp"
#include "rWorld.hpp"
#include "uLog.hpp"

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
   VkBuffer     lVertex    = vVertex.getBuffer();

   vShader->cmdBindDescriptorSets( _buf, VK_PIPELINE_BIND_POINT_GRAPHICS );
   vPipeline->cmdBindPipeline( _buf, VK_PIPELINE_BIND_POINT_GRAPHICS );

   if ( vHasModelMatrix_PC ) {
      std::lock_guard<std::recursive_mutex> lLock( vMatrixAccess );
      vShader->cmdUpdatePushConstant( _buf, vMatrixModelVar_PC, getModelMatrix()->getMatrix() );
   }

   vkCmdBindVertexBuffers( _buf, vPipeline->getVertexBindPoint(), 1, &lVertex, &lOffsets[0] );
   vkCmdBindIndexBuffer( _buf, vIndex.getBuffer(), 0, VK_INDEX_TYPE_UINT32 );
   vkCmdDrawIndexed( _buf, vIndex.getSize(), 1, 0, 0, 1 );
}

/*!
 * \brief Inits the object (partialy)
 * \note This function SHOULD NOT be called directly! Use the functions in rScene instead!
 */
std::vector<rBuffer *> rSimpleMesh::setData_IMPL( VkCommandBuffer              _buf,
                                                  const std::vector<uint32_t> &_index,
                                                  const std::vector<float> &   _data ) {
   iLOG( "Initializing simple mesh object ", vName_str );

   vIndex.cmdInit( _index, _buf, VK_BUFFER_USAGE_INDEX_BUFFER_BIT );
   vVertex.cmdInit( _data, _buf, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT );

   return {&vIndex, &vVertex};
}

void rSimpleMesh::signalRenderReset( internal::rRendererBase * ) {
   if ( !vPipeline ) {
      eLOG( "Pipeline not setup!" );
      return;
   }

   vShader      = getShader();
   vVertUniform = vShader->getUniformBuffer( VK_SHADER_STAGE_VERTEX_BIT );

   auto lPushConstants = vShader->getPushConstants( VK_SHADER_STAGE_VERTEX_BIT );

   if ( !vVertUniform ) {
      wLOG( "No uniform buffers in shader" );
      return;
   }

   vHasMVPMatrix = false;

   for ( auto const &i : vVertUniform->vars ) {
      if ( i.guessedRole == rShaderBase::MODEL_VIEW_PROJECTION_MATRIX ) {
         vHasMVPMatrix = vShader->tryReserveUniform( i );
         vMatrixMVPVar = i;
         break;
      }

      if ( i.guessedRole == rShaderBase::VIEW_PROJECTION_MATRIX ) {
         vHasVPMatrix = vShader->tryReserveUniform( i );
         vMatrixVPVar = i;
         break;
      }
   }

   for ( auto const &i : lPushConstants ) {
      if ( i.guessedRole == rShaderBase::MODEL_MATRIX ) {
         vHasModelMatrix_PC = true;
         vMatrixModelVar_PC = i;
      }
   }
}

void rSimpleMesh::updateUniforms() {
   if ( !vPipeline || !vShader ) {
      eLOG( "Pipeline / shader not setup!" );
      return;
   }

   if ( vHasMVPMatrix ) {
      std::lock_guard<std::recursive_mutex> lLock( vMatrixAccess );
      vShader->updateUniform( vMatrixMVPVar, getModelViewProjectionMatrix()->getMatrix() );
   }

   if ( vHasVPMatrix ) {
      std::lock_guard<std::recursive_mutex> lLock( vMatrixAccess );
      vShader->updateUniform( vMatrixVPVar, getViewProjectionMatrix()->getMatrix() );
   }
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
