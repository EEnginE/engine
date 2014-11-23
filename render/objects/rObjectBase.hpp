/*!
 * \file rObjectBase.hpp
 */

#ifndef R_OBJECT_BASE_HPP
#define R_OBJECT_BASE_HPP

#include <string>
#include <GL/glew.h>
#include "rLoaderBase.hpp"
#include "rMatrixMath.hpp"

namespace e_engine {

// Object Flags:
#define MESH_OBJECT     ( 1 << 0 )
#define LIGHT_SOURCE    ( 1 << 1 )
#define AMBIENT_LIGHT   ( 1 << 2 )

// Matrix Flags:
#define SCALE_MATRIX_FLAG       ( 1 << 0 )
#define ROTATION_MATRIX_FLAG    ( 1 << 1 )
#define TRANSLATION_MATRIX_FLAG ( 1 << 2 )
#define CAMERA_MATRIX_FLAG      ( 1 << 3 )
#define OBJECT_MATRIX_FLAG      ( 1 << 4 )
#define FINAL_MATRIX_FLAG       ( 1 << 5 )


/*!
 * \brief Base class for creating objects
 *
 * You MUST set some hints in your constructor about the object and create a clearOGLData__
 * and setOGLData__ function.
 *
 * The data will be loaded into RAM from this class and can than be accessed via vLoaderData
 *
 * You should also return 1 if everything went fine. Values < 0 mean there where errors but data
 * can be safely set / cleared. Values > 1 mean that the data can still be used and may be successfully
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
         IS_DATA_READY,
         __LAST__
      };

      enum DATA_FILE_TYPE { AUTODETECT, OBJ_FILE, SET_DATA_MANUALLY };

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
         CAMERA_SPACE,
         OBJECT_SPACE,
         FINAL
      };

      enum VECTOR_TYPES {
         AMBIENT_COLOR
      };

   protected:
      uint64_t vObjectHints[__LAST__];
      std::string vName_str;

      std::string vFile_str;
      DATA_FILE_TYPE vFileType;

      bool vIsLoaded_B;
      bool vKeepDataInRAM_B;

      internal::rLoaderBase<float> *vLoaderData;

      DATA_FILE_TYPE detectFileTypeFromEnding( std::string const &_str );

      virtual int clearOGLData__() = 0;
      virtual int setOGLData__()   = 0;
   public:
      rObjectBase( std::string _name, std::string _file, DATA_FILE_TYPE _type = AUTODETECT ) :
         vName_str( _name ),
         vFile_str( _file ),
         vFileType( _type ),
         vKeepDataInRAM_B( false ),
         vLoaderData( nullptr ),
         vIsLoaded_B( false ) {
         for ( uint32_t i = 0; i < __LAST__; ++i )
            vObjectHints[i] = 0;
      }

      rObjectBase() = delete;

      // Forbid copying
      rObjectBase( const rObjectBase & ) = delete;
      rObjectBase &operator=( const rObjectBase & ) = delete;

      // Allow moving
      rObjectBase( rObjectBase&& ) {}
      rObjectBase &operator=( rObjectBase&& ) {return *this;}

      virtual ~rObjectBase() {clearRAMData();}

      int  loadData();
      void clearRAMData();
      int  clearAllData();

      int clearOGLData();
      int setOGLData();

      template<class... ARGS>
      inline void getHints( OBJECT_HINTS _hint, int &_ret, ARGS&&... _args );

      inline void getHints( OBJECT_HINTS _hint, int &_ret );

      bool getIsDataInRAM()  const { if ( vLoaderData ) return true; return false; }
      bool getIsDataLoaded() const { return vIsLoaded_B; }

      void setKeepDataInRAM( bool _keep ) { vKeepDataInRAM_B = _keep; }

      std::string getName()  const { return vName_str; }

      virtual uint32_t getVBO( GLuint &_n );
      virtual uint32_t getIBO( GLuint &_n );

      virtual uint32_t getMatrix( rMat4f **_mat, MATRIX_TYPES _type );
      virtual uint32_t getMatrix( rMat4d **_mat, MATRIX_TYPES _type );

      virtual uint32_t getVector( rVec4f **_vec, VECTOR_TYPES _type );
      virtual uint32_t getVector( rVec4d **_vec, VECTOR_TYPES _type );
};

template<class... ARGS>
void rObjectBase::getHints( OBJECT_HINTS _hint, int &_ret, ARGS&&... _args ) {
   _ret = vObjectHints[_hint];
   getHints( std::forward<ARGS>( _args )... );
}

void rObjectBase::getHints( OBJECT_HINTS _hint, int &_ret ) {
   _ret = vObjectHints[_hint];
}


} // e_engine

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
