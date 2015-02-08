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
#include <array>
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
 * successfully
 * cleared later. Value = 0 means that this object is completely broken!
 *
 * \warning Always inherit this class FIRST
 */
class rObjectBase {
 public:
   enum OBJECT_HINTS {
      FLAGS,
      MATRICES,
      LIGHT_MODEL,
      IS_DATA_READY,

      DATA_BUFFER,
      INDEX_BUFFER,

      NUM_MESHS,  //!< Number of mesh targets
      NUM_LINES,  //!< Number of line targets
      NUM_POINTS, //!< Number of point targets
      __LAST__,

      INDEX_OFFSET,
      INDEX_STRIDE,
      NUM_INDEXES,

      VERTEX_OFFSET,
      VERTEX_STRIDE,
      NORMAL_OFFSET,
      NORMAL_STRIDE,
      UV_OFFSET,
      UV_STRIDE,

      MESH_TYPE,

      __LAST_OBJECT__
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
   std::array<int64_t, __LAST__> vObjectHints;
   std::vector<std::array<int64_t, __LAST_OBJECT__ - __LAST__>> vObjects;
   std::string vName_str;

   bool vIsLoaded_B;

   virtual int clearOGLData__() = 0;
   virtual int setOGLData__() = 0;

 private:
   template <class... ARGS>
   inline bool getHintsOBJ( size_t _obj, OBJECT_HINTS _hint, int64_t &_ret, ARGS &&... _args );
   inline bool getHintsOBJ( size_t _obj, OBJECT_HINTS _hint, int64_t &_ret );

 public:
   rObjectBase( std::string _name ) : vName_str( _name ), vIsLoaded_B( false ) {
      for ( auto &i : vObjectHints )
         i = -1;
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
   inline bool getHints( OBJECT_HINTS _hint, int64_t &_ret, ARGS &&... _args );
   inline bool getHints( OBJECT_HINTS _hint, int64_t &_ret );

   template <class... ARGS>
   inline bool getHintsOBJ( MESH_TYPES _type, size_t _obj, ARGS &&... _args );

   bool getIsDataLoaded() const { return vIsLoaded_B; }
   std::string getName() const { return vName_str; }

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
bool rObjectBase::getHints( OBJECT_HINTS _hint, int64_t &_ret, ARGS &&... _args ) {
   if ( _hint < __LAST__ )
      _ret = vObjectHints[_hint];
   else
      return false;

   return getHints( std::forward<ARGS>( _args )... );
}

bool rObjectBase::getHints( OBJECT_HINTS _hint, int64_t &_ret ) {
   if ( _hint < __LAST__ )
      _ret = vObjectHints[_hint];
   else
      return false;

   return true;
}

template <class... ARGS>
bool rObjectBase::getHintsOBJ( size_t _obj, OBJECT_HINTS _hint, int64_t &_ret, ARGS &&... _args ) {
   if ( _hint < __LAST_OBJECT__ && _hint > __LAST__ )
      _ret = vObjects[_obj][_hint - __LAST__];
   else
      return false;

   return getHintsOBJ( _obj, std::forward<ARGS>( _args )... );
}

bool rObjectBase::getHintsOBJ( size_t _obj, OBJECT_HINTS _hint, int64_t &_ret ) {
   if ( _hint < __LAST_OBJECT__ && _hint > __LAST__ )
      _ret = vObjects[_obj][_hint - __LAST__];
   else
      return false;

   return true;
}

template <class... ARGS>
bool rObjectBase::getHintsOBJ( MESH_TYPES _type, size_t _obj, ARGS &&... _args ) {
   switch ( _type ) {
      case MESH_3D:
         if ( vObjectHints[NUM_MESHS] <= static_cast<int64_t>( _obj ) )
            return false;

         break;
      case LINES_3D:
         if ( vObjectHints[NUM_LINES] <= static_cast<int64_t>( _obj ) )
            return false;

         break;

      case POINTS_3D:
         if ( vObjectHints[NUM_POINTS] <= static_cast<int64_t>( _obj ) )
            return false;

         break;
   }

   size_t lTemp = 0;
   _obj += 1; // Is a bit ugly but the best solution beacuse _obj is unsigned

   for ( ; lTemp < vObjects.size(); ++lTemp ) {
      if ( vObjects[lTemp][MESH_TYPE - __LAST__] == _type )
         --_obj;

      if ( _obj == 0 )
         return getHintsOBJ( lTemp, std::forward<ARGS>( _args )... );
   }

   eLOG( "WHAT!?!?!" );

   // Object not found
   return false;
}


} // e_engine

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
