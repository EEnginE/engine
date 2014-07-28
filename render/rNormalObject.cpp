/*!
 * \file rNormalObject.cpp
 * \brief \b Classes: \a rNormalObject
 */

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include "rNormalObject.hpp"
#include "uLog.hpp"
#include "math.h"


// Renderer
#include "rRenderNormal_3_3.hpp"

namespace e_engine {

namespace {
float degToRad( float _degree ) { return _degree * ( E_VAR_PI / 180.0 ); }
}

rNormalObject::rNormalObject( std::string _name ) {
   vIsDataLoaded_B        = false;
   vHasGeneretedBuffers_B = false;
   vRenderer              = nullptr;
   vObjectName            = _name;

   vResultMatrix          = nullptr;

   vTransformMatrix.toIdentityMatrix();
   vRotateMatrix.toIdentityMatrix();

   vRotateX = 0;
   vRotateY = 0;
   vRotateZ = 0;
}

rNormalObject::~rNormalObject() {
   if( vRenderer != nullptr )
      delete vRenderer;

   freeData();
}

rNormalObject::DATA_FILE_TYPE rNormalObject::detectFileTypeFromEnding( std::string const &_str ) {
   boost::regex lDataEndingOBJ_ex( "\\.obj" );

   if( boost::regex_match( _str.end() - 4, _str.end(), lDataEndingOBJ_ex ) ) {
      return OBJ_FILE;
   }

   return AUTODETECT; // failed
}

/*!
 * \brief Adds a data file to the object
 * \param[in] _pathToDataFile the path to the file
 * \param[in] _type           the type of the file
 * \returns true if everything went fine
 * \returns false if the file does not exists or is readable
 */
bool rNormalObject::addData( std::string _pathToDataFile, e_engine::rNormalObject::DATA_FILE_TYPE _type ) {
   if( _type == AUTODETECT )
      _type = detectFileTypeFromEnding( _pathToDataFile );

   if( _type == AUTODETECT ) {
      eLOG "Failed to autodetect file type from suffix! (" ADD _pathToDataFile ADD ")" END
      return false;
   }

   boost::filesystem::path lPath( _pathToDataFile.c_str() );

   if( ! boost::filesystem::exists( lPath ) ) {
      eLOG "Data File '" ADD _pathToDataFile ADD "' does not exists" END
      return false;
   }

   if( ! boost::filesystem::is_regular_file( lPath ) ) {
      eLOG "Data File '" ADD _pathToDataFile ADD "' is not a regular file" END
      return false;
   }

   vDataFiles.emplace_back( _pathToDataFile, _type );
   return true;
}

bool rNormalObject::addShader( std::string _shaderPath ) {
   vShaders.emplace_back( _shaderPath );
   return true;
}


void rNormalObject::freeData() {
   if( !( vHasGeneretedBuffers_B || vIsDataLoaded_B ) )

      if( !iInit::isAContextCurrentForThisThread() ) {
         eLOG    "Can NOT FREE data because no OpenGL context is current for this thread!"
         NEWLINE "This function may be called from the destructor!"
         END
         return;
      }

   if( vHasGeneretedBuffers_B ) {
      for( GLuint & i : vVertexBufferObjects )
         glDeleteBuffers( 1, &i );

      for( GLuint & i : vIndexBufferObjects )
         glDeleteBuffers( 1, &i );
   }

   for( rShader & s : vShaders ) {
      if( s.getIsLinked() )
         s.deleteProgram();
   }

   vHasGeneretedBuffers_B = false;
   vIsDataLoaded_B        = true;
}

/*!
 * \brief Loads the content of the object
 *
 * This function loads the content of the object and prepares it for
 * rendering.
 *
 * \warning This function needs an \b ACTIVE OpenGL context for THIS THREAD
 *
 * \returns 1  if everything went fine
 * \returns 0  if there is no OpenGL context current for this thead
 * \returns -1 if the data could not be loaded / parsed
 * \returns -2 if the shaders are useless
 * \returns -3 if data is already loaded
 * \returns 5  if there is no renderer for this object type
 */
int rNormalObject::loadData( rWorld *_world ) {
   if( !iInit::isAContextCurrentForThisThread() ) {
      eLOG "Can not init data because no OpenGL context is current for this thread!" END
      return 0;
   }

   if( vHasGeneretedBuffers_B || vIsDataLoaded_B )
      return -3;

   std::vector<RENDERER_ID> lPossibleRendere;

   // compile shaders
   for( rShader & s : vShaders ) {
      if( ! s.getIsLinked() )  {
         if( s.compile() < 0 ) {
            eLOG "Failed to compile shader '" ADD s.getShaderPath() ADD "'. Failed to init OpenGL for object'" ADD vObjectName ADD "'" END
            return -1;
         }
      }

      vShaderInfo.emplace_back( s.getShaderInfo() );
   }

   vHasGeneretedBuffers_B = true;

   vVertexBufferObjects.resize( vDataFiles.size() );
   vIndexBufferObjects.resize( vDataFiles.size() );

   for( GLuint & i : vVertexBufferObjects )
      glGenBuffers( 1, &i );

   for( GLuint & i : vIndexBufferObjects )
      glGenBuffers( 1, &i );


   unsigned int lCounter = 0;

   std::vector<GLuint> lIndexSize;

   rLoader_3D_f_OBJ lLoader;

   for( dataFile const & d : vDataFiles ) {
      switch( d.type ) {
         case OBJ_FILE:
            lLoader.setFile( d.path ) ;
            if( lLoader.load() != 1 ) {
               eLOG "Failed to load / parse the OBJ! ('" ADD d.path ADD "')" END
               return -1;
            }

            glBindBuffer( GL_ARRAY_BUFFER,         vVertexBufferObjects[lCounter] );
            glBufferData( GL_ARRAY_BUFFER,         sizeof( GLfloat ) * lLoader.getRawVertexData()->size(), &lLoader.getRawVertexData()->at( 0 ), GL_STATIC_DRAW );

            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObjects[lCounter] );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLuint ) * lLoader.getRawIndexData()->size(),   &lLoader.getRawIndexData()->at( 0 ), GL_STATIC_DRAW );
            lIndexSize.emplace_back( lLoader.getRawIndexData()->size() );
            lLoader.unLoad();
            break;
         case AUTODETECT:
            eLOG "Data type for '" ADD d.path ADD "' is still AUTODETECT -- it should be impossible to see this message" END
            return -1;
         default:
            eLOG "Unkown file type for '" ADD d.path ADD "' -- it should be impossible to see this message" END
            return -1;
      }
      ++lCounter;
   }


   // Now choose a renderer


   std::vector<void *> lTempData;
   GLuint              lTempShaderID;

   switch( chooseRendere() ) {
      case render_OGL_3_3_Normal_Basic_1S_1D:
         vRenderer = new rRenderNormal_3_3();

         vShaders[0].getProgram( lTempShaderID );

         lTempData.emplace_back( ( void * ) & ( vVertexBufferObjects[0] ) );
         lTempData.emplace_back( ( void * ) & ( vIndexBufferObjects[0] ) );
         lTempData.emplace_back( ( void * ) & ( lTempShaderID ) );
         lTempData.emplace_back( ( void * ) & ( vInputLocations[0] ) );
         lTempData.emplace_back( ( void * ) & ( vUniformLocations[0] ) );
         lTempData.emplace_back( ( void * ) & ( lIndexSize[0] ) );

         vRenderer->setOGLInfo( lTempData );

         vResultMatrix = vRenderer->getMatrix();
         break;

      case render_NONE:
         eLOG "No renderer for this object type '" ADD vObjectName ADD "'" END
         return 5;
   }


   vProjectionMatrix = _world->getProjectionMatrix();


   iLOG "Loaded data for '" ADD vObjectName ADD "'" END

   vIsDataLoaded_B = true;

   return 1;
}

/*!
 * \brief Decides which renderer to take
 *
 *
 */
RENDERER_ID rNormalObject::chooseRendere() {
   std::vector<RENDERER_ID> lPossibleRendere;
   lPossibleRendere.emplace_back( render_NONE );

   if( vShaders.size() == 1 && vDataFiles.size() == 1 ) {
      if( vShaderInfo[0].vInputInfo.size() == 1 && vShaderInfo[0].vUniformInfo.size() == 1 ) {
         if( vShaderInfo[0].vUniformInfo[0].type == GL_FLOAT_MAT4 ) {
            vInputLocations.emplace_back( vShaderInfo[0].vInputInfo[0].location );
            vUniformLocations.emplace_back( vShaderInfo[0].vUniformInfo[0].location );

            lPossibleRendere.emplace_back( render_OGL_3_3_Normal_Basic_1S_1D );
         }
      }
   }


   return lPossibleRendere.back();
}

void rNormalObject::createResultMatrix() {
   if( vResultMatrix == nullptr || vRenderer == nullptr || vProjectionMatrix == nullptr )
      return;

//    rMatrix<4, 4> lTemp = *vProjectionMatrix * vTransformMatrix * vRotateMatrix  ;
// 
//    for( unsigned int i = 0; i < 4; ++i ) {
//       std::string lStr;
//       for( unsigned int j = 0; j < 4; ++j ) {
//          lStr += boost::lexical_cast<std::string>( lTemp.get( j, i ) ) + "  ";
//       }
//       iLOG lStr END
//    }
//    
//    dLOG "" END
// 
//    for( unsigned int i = 0; i < 4; ++i ) {
//       std::string lStr;
//       for( unsigned int j = 0; j < 4; ++j ) {
//          lStr += boost::lexical_cast<std::string>( vTransformMatrix.get( j, i ) ) + "  ";
//       }
//       wLOG lStr END
//    }
//    
//    dLOG "" END
//    
//    for( unsigned int i = 0; i < 4; ++i ) {
//       std::string lStr;
//       for( unsigned int j = 0; j < 4; ++j ) {
//          lStr += boost::lexical_cast<std::string>( vRotateMatrix.get( j, i ) ) + "  ";
//       }
//       wLOG lStr END
//    }
//    
//    dLOG "" END
// 
//    for( unsigned int i = 0; i < 4; ++i ) {
//       std::string lStr;
//       for( unsigned int j = 0; j < 4; ++j ) {
//          lStr += boost::lexical_cast<std::string>( vProjectionMatrix->get( j, i ) ) + "  ";
//       }
//       eLOG lStr END
//    }
//    
//    dLOG "" END
//    dLOG "" END

   *vResultMatrix = *vProjectionMatrix * vTransformMatrix * vRotateMatrix;
   vRenderer->updateUniforms();
}


void rNormalObject::setRotation( GLfloat _x, GLfloat _y, GLfloat _z ) {
   rMatrix<4, 4> lTempX;
   rMatrix<4, 4> lTempY;
   rMatrix<4, 4> lTempZ;
   
   vRotateX = _x;
   vRotateY = _y;
   vRotateZ = _z;

   GLfloat radX = degToRad( _x );
   GLfloat radY = degToRad( _z );
   GLfloat radZ = degToRad( _y );
   
   lTempX.toIdentityMatrix();
   lTempY.toIdentityMatrix();
   lTempZ.toIdentityMatrix();


   lTempX.set( 1, 1, cos( radX ) );
   lTempX.set( 1, 2, sin( radX ) );

   lTempX.set( 2, 1, - sin( radX ) );
   lTempX.set( 2, 2, cos( radX ) );


   lTempY.set( 0, 0, cos( radY ) );
   lTempY.set( 0, 2, sin( radY ) );

   lTempY.set( 2, 0, - sin( radY ) );
   lTempY.set( 2, 2, cos( radY ) );


   lTempZ.set( 0, 0, cos( radZ ) );
   lTempZ.set( 0, 1, sin( radZ ) );

   lTempZ.set( 1, 0, -sin( radZ ) );
   lTempZ.set( 1, 1, cos( radZ ) );
      
   vRotateMatrix = lTempZ * lTempY * lTempX;
}



std::vector< std::string > rNormalObject::getDataFileNames() {
   std::vector<std::string> lTemp;
   for( dataFile const & d : vDataFiles ) {
      lTemp.emplace_back( d.path );
   }
   return lTemp;
}

std::vector< rShader > *rNormalObject::getShaders() {
   return &vShaders;
}

void rNormalObject::reset() {
   freeData();
   vShaders.clear();
   vDataFiles.clear();
}





}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
