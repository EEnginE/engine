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

#ifndef R_OBJECT_BASE_HPP
#define R_OBJECT_BASE_HPP

#include "defines.hpp"

#include <string>
#include <GL/glew.h>
#include "rLoaderBase.hpp"
#include "rMatrixMath.hpp"

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
 *successfully
 * cleared later. Value = 0 means that this object is completely broken!
 *
 */
class rObjectBase {
 public:
   enum OBJECT_HINTS {
      FLAGS,
      MATRICES,
      NUM_VERTICES,
      NUM_INDEXES,
      NUM_NORMALS,
      LIGHT_MODEL,
      NUM_VBO,
      NUM_IBO,
      NUM_NBO,
      IS_DATA_READY,
      __LAST__
   };

   enum ERROR_FLAGS {
      ALL_OK = 0,
      FUNCTION_NOT_VALID_FOR_THIS_OBJECT = ( 1 << 0 ),
      INDEX_OUT_OF_RANGE = ( 1 << 1 ),
      DATA_NOT_LOADED = ( 1 << 2 ),
      UNSUPPORTED_TYPE = ( 1 << 3 ),
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

 protected:
   uint64_t vObjectHints[__LAST__];
   std::string vName_str;

   bool vIsLoaded_B;

   virtual int clearOGLData__() = 0;
   virtual int setOGLData__() = 0;

 public:
   rObjectBase( std::string _name ) : vName_str( _name ), vIsLoaded_B( false ) {
      for ( uint32_t i = 0; i < __LAST__; ++i )
         vObjectHints[i] = 0;
   }

   rObjectBase() = delete;

   // Forbid copying
   rObjectBase( const rObjectBase & ) = delete;
   rObjectBase &operator=( const rObjectBase & ) = delete;

   // Allow moving
   rObjectBase( rObjectBase && ) {}
   rObjectBase &operator=( rObjectBase && ) { return *this; }

   virtual ~rObjectBase();

   int clearOGLData();
   int setOGLData();

   template <class... ARGS>
   inline void getHints( OBJECT_HINTS _hint, uint64_t &_ret, ARGS &&... _args );

   inline void getHints( OBJECT_HINTS _hint, uint64_t &_ret );

   bool getIsDataLoaded() const { return vIsLoaded_B; }

   std::string getName() const { return vName_str; }

   virtual uint32_t getVBO( GLuint &_n );
   virtual uint32_t getIBO( GLuint &_n );
   virtual uint32_t getNBO( GLuint &_n );

   virtual uint32_t getMatrix( rMat4f **_mat, MATRIX_TYPES _type );
   virtual uint32_t getMatrix( rMat4d **_mat, MATRIX_TYPES _type );

   virtual uint32_t getMatrix( rMat3f **_mat, MATRIX_TYPES _type );
   virtual uint32_t getMatrix( rMat3d **_mat, MATRIX_TYPES _type );

   virtual uint32_t getVector( rVec4f **_vec, VECTOR_TYPES _type );
   virtual uint32_t getVector( rVec4d **_vec, VECTOR_TYPES _type );

   virtual uint32_t getVector( rVec3f **_vec, VECTOR_TYPES _type );
   virtual uint32_t getVector( rVec3d **_vec, VECTOR_TYPES _type );
};

template <class... ARGS>
void rObjectBase::getHints( OBJECT_HINTS _hint, uint64_t &_ret, ARGS &&... _args ) {
   _ret = vObjectHints[_hint];
   getHints( std::forward<ARGS>( _args )... );
}

void rObjectBase::getHints( OBJECT_HINTS _hint, uint64_t &_ret ) { _ret = vObjectHints[_hint]; }


} // e_engine

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
