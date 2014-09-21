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


rNormalObject::rNormalObject( std::string _name ) {
   vIsDataLoaded_B        = false;
   vHasGeneretedBuffers_B = false;
   vRenderer              = nullptr;
   vObjectName            = _name;
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
      eLOG( "Failed to autodetect file type from suffix! (", _pathToDataFile, ")" );
      return false;
   }

   boost::filesystem::path lPath( _pathToDataFile.c_str() );

   if( ! boost::filesystem::exists( lPath ) ) {
      eLOG( "Data File '", _pathToDataFile, "' does not exist" );
      return false;
   }

   if( ! boost::filesystem::is_regular_file( lPath ) ) {
      eLOG( "Data File '", _pathToDataFile, "' is not a regular file" );
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
         eLOG( "Can NOT FREE data because no OpenGL context is current for this thread!\nThis function may be called from the destructor!" );
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
      eLOG( "Can not init data because no OpenGL context is current for this thread!" );
      return 0;
   }

   if( vHasGeneretedBuffers_B || vIsDataLoaded_B )
      return -3;

   std::vector<RENDERER_ID> lPossibleRender;

   // compile shaders
   for( rShader & s : vShaders ) {
      if( ! s.getIsLinked() )  {
         if( s.compile() < 0 ) {
            eLOG( "Failed to compile shader '", s.getShaderPath(), "'. Failed to init OpenGL for object'", vObjectName, "'" );
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
               eLOG( "Failed to load / parse the OBJ! ('", d.path, "')" );
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
            eLOG( "Data type for '", d.path, "' is still AUTODETECT -- it should be impossible to see this message" );
            return -1;
         default:
            eLOG( "Unkown file type for '", d.path, "' -- it should be impossible to see this message" );
            return -1;
      }
      ++lCounter;
   }

   // Now choose a renderer


   std::vector<void *> lTempData;
   GLuint              lTempShaderID;

   switch( chooseRender() ) {
      case render_OGL_3_3_Normal_Basic_1S_1D:
         vRenderer = new rRenderNormal_3_3( getFinalMatrix() );

         vShaders[0].getProgram( lTempShaderID );

         lTempData.emplace_back( ( void * ) & ( vVertexBufferObjects[0] ) );
         lTempData.emplace_back( ( void * ) & ( vIndexBufferObjects[0] ) );
         lTempData.emplace_back( ( void * ) & ( lTempShaderID ) );
         lTempData.emplace_back( ( void * ) & ( vInputLocations[0] ) );
         lTempData.emplace_back( ( void * ) & ( vUniformLocations[0] ) );
         lTempData.emplace_back( ( void * ) & ( lIndexSize[0] ) );

         vRenderer->setOGLInfo( lTempData );
         break;

      case render_NONE:
         eLOG( "No renderer for this object type '", vObjectName, "'" );
         return 5;
   }


   setCmaraSpaceMatrix( _world->getCameraSpaceMatrix() );


   iLOG( "Loaded data for '", vObjectName, "'" );

   vIsDataLoaded_B = true;

   return 1;
}

/*!
 * \brief Decides which renderer to use
 *
 *
 */
RENDERER_ID rNormalObject::chooseRender() {
   std::vector<RENDERER_ID> lPossibleRender;
   lPossibleRender.emplace_back( render_NONE );

   if( vShaders.size() == 1 && vDataFiles.size() == 1 ) {
      if( vShaderInfo[0].vInputInfo.size() == 1 && vShaderInfo[0].vUniformInfo.size() == 1 ) {
         if( vShaderInfo[0].vUniformInfo[0].type == GL_FLOAT_MAT4 ) {
            vInputLocations.emplace_back( vShaderInfo[0].vInputInfo[0].location );
            vUniformLocations.emplace_back( vShaderInfo[0].vUniformInfo[0].location );

            lPossibleRender.emplace_back( render_OGL_3_3_Normal_Basic_1S_1D );
         }
      }
   }


   return lPossibleRender.back();
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
