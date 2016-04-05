/*!
 * \file rObjectBase.hpp
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

#include <string>
#include <array>
#include <vulkan.h>
#include "lLoaderBase.hpp"
#include "rMatrixMath.hpp"
#include "rBuffer.hpp"

namespace e_engine {

// Object Flags:
#define MESH_OBJECT ( 1 << 0 )

#define LIGHT_SOURCE ( 1 << 1 )
#define DIRECTIONAL_LIGHT ( 1 << 2 )
#define POINT_LIGHT ( 1 << 3 )
#define SPOT_LIGHT ( 1 << 4 )

// Matrix Flags:
#define SCALE_MATRIX_FLAG ( 1 << 0 )
#define ROTATION_MATRIX_FLAG ( 1 << 1 )
#define TRANSLATION_MATRIX_FLAG ( 1 << 2 )
#define VIEW_MATRIX_FLAG ( 1 << 3 )
#define PROJECTION_MATRIX_FLAG ( 1 << 4 )
#define CAMERA_MATRIX_FLAG ( 1 << 5 )
#define MODEL_MATRIX_FLAG ( 1 << 6 )
#define MODEL_VIEW_MATRIX_FLAG ( 1 << 7 )
#define MODEL_VIEW_PROJECTION_MATRIX_FLAG ( 1 << 8 )
#define NORMAL_MATRIX_FLAG ( 1 << 9 )

class rPipeline;
class rShaderBase;

/*!
 * \brief Base class for creating objects
 *
 * You MUST set some hints in your constructor about the object and create a clearOGLData__
 * and setOGLData__ function.
 *
 * The data will be loaded into RAM from this class and can than be accessed via vLoaderData
 *
 * You should also return 1 if everything went fine. Values < 0 mean there where errors but data
 * can be safely set / cleared. Values > 1 mean that the data can still be used and may be
 * successfully
 * cleared later. Value = 0 means that this object is completely broken!
 *
 * \warning Always inherit this class FIRST
 */
class RENDER_API rObjectBase {
 public:
   enum ERROR_FLAGS {
      ALL_OK                             = 0,
      FUNCTION_NOT_VALID_FOR_THIS_OBJECT = ( 1 << 0 ),
      INDEX_OUT_OF_RANGE                 = ( 1 << 1 ),
      DATA_NOT_LOADED                    = ( 1 << 2 ),
      UNSUPPORTED_TYPE                   = ( 1 << 3 ),
   };

   enum MATRIX_TYPES {
      SCALE,
      ROTATION,
      TRANSLATION,
      VIEW_MATRIX,
      PROJECTION_MATRIX,
      CAMERA_MATRIX,
      MODEL_MATRIX,
      MODEL_VIEW_MATRIX,
      MODEL_VIEW_PROJECTION,
      NORMAL_MATRIX
   };

   enum VECTOR_TYPES {
      AMBIENT_COLOR,
      LIGHT_COLOR,
      POSITION,
      POSITION_MODEL_VIEW,
      DIRECTION,
      ATTENUATION
   };

   enum LIGHT_MODEL_T { NO_LIGHTS = 0, SIMPLE_ADS_LIGHT };

 private:
   std::vector<rBuffer *> vLoadBuffers;

 protected:
   std::string vName_str;

   bool vPartialLoaded_B = false;
   bool vIsLoaded_B      = false;
   rPipeline *vPipeline  = nullptr;

   virtual std::vector<rBuffer *> setData_IMPL( VkCommandBuffer,
                                                std::vector<uint32_t> const &,
                                                std::vector<float> const &,
                                                std::vector<float> const &,
                                                std::vector<float> const & ) {
      return {};
   };

 public:
   rObjectBase( std::string _name ) : vName_str( _name ) {}
   rObjectBase() = delete;

   // Forbid copying
   rObjectBase( const rObjectBase & ) = delete;
   rObjectBase &operator=( const rObjectBase & ) = delete;

   // Allow moving
   rObjectBase( rObjectBase && ) = default;
   rObjectBase &operator=( rObjectBase && ) = default;

   virtual ~rObjectBase();

   bool setData( VkCommandBuffer _buf,
                 std::vector<uint32_t> const &_index,
                 std::vector<float> const &_pos,
                 std::vector<float> const &_norm,
                 std::vector<float> const &_uv );

   bool finishData();

   virtual bool checkIsCompatible( rPipeline *_pipe ) = 0;
   virtual bool canRecord() = 0;
   virtual void updateUniforms() {}
   virtual void record( VkCommandBuffer ) {}
   virtual void signalRenderReset() {}

   rPipeline *getPipeline() { return vPipeline; }
   rShaderBase *getShader();
   bool getIsDataLoaded() const { return vIsLoaded_B; }
   std::string getName() const { return vName_str; }
   bool setPipeline( rPipeline *_pipe );

   virtual uint32_t getMatrix( rMat4f **_mat, MATRIX_TYPES _type );
   virtual uint32_t getMatrix( rMat4d **_mat, MATRIX_TYPES _type );

   virtual uint32_t getMatrix( rMat3f **_mat, MATRIX_TYPES _type );
   virtual uint32_t getMatrix( rMat3d **_mat, MATRIX_TYPES _type );

   virtual uint32_t getVector( rVec4f **_vec, VECTOR_TYPES _type );
   virtual uint32_t getVector( rVec4d **_vec, VECTOR_TYPES _type );

   virtual uint32_t getVector( rVec3f **_vec, VECTOR_TYPES _type );
   virtual uint32_t getVector( rVec3d **_vec, VECTOR_TYPES _type );

   bool setupVertexData_PN( std::vector<float> const &_pos,
                            std::vector<float> const &_norm,
                            std::vector<float> &_out );
};



} // e_engine



// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
