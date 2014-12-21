/*!
 * \file rScene.cpp
 */

#include "rScene.hpp"
#include "uLog.hpp"

namespace e_engine {


rSceneBase::~rSceneBase() {
   for( auto & o : vObjects ) {
      if( o.vRenderer ) {
         delete o.vRenderer;
         o.vRenderer = nullptr;
      }
   }
}

/*!
 * \brief Tests if it is safe to render the scene
 * \returns true when it is safe
 */
bool rSceneBase::canRenderScene() {
   bool lCanRender = true;
   for( auto const & d : vObjects ) {
      int lIsObjectReady, lFlags;

      if( !d.vObjectPointer ) {
         wLOG( "Invalid Object Pointer" );
         lCanRender = false;
         continue;
      }

      d.vObjectPointer->getHints(
            rObjectBase::IS_DATA_READY, lIsObjectReady,
            rObjectBase::FLAGS,         lFlags
      );

      if( lIsObjectReady != GL_TRUE ) {
         wLOG( "Object data for '", d.vObjectPointer->getName(), "' is not completely loaded --> Do not render scene '", vName_str, "'" );
         lCanRender = false;
         continue;
      }

      if( lFlags & MESH_OBJECT ) {
         if( !d.vRenderer ) {
            wLOG( "No Renderer set for '", d.vObjectPointer->getName(), "' --> Do not render scene '", vName_str, "'" );
            lCanRender = false;
            continue;
         }

         if( !d.vRenderer->canRender() ) {
            wLOG( "Render can not render this object '", d.vObjectPointer->getName(), "' --> Do not render scene '", vName_str, "'" );
            lCanRender = false;
            continue;
         }

         iLOG( "Solid object ready for rendering: '", d.vObjectPointer->getName(), "'" );
      }

      if( lFlags & LIGHT_SOURCE || lFlags & AMBIENT_LIGHT ) {
         iLOG( "Light source object ready for rendering: '", d.vObjectPointer->getName(), "'" );
      }
   }

   for( auto const & d : vShaders ) {
      if( !d.getIsLinked() ) {
         wLOG( "Shader '", d.getShaderPath(), "' is not compiled / linked --> Do not render scene '", vName_str, "'" );
         lCanRender = false;
      }
   }

   if( lCanRender )
      iLOG( "Scene '", vName_str, "' with ", vObjects.size(), " objects ready for rendering" );

   return lCanRender;
}

/*!
 * \brief Renders the scene
 *
 * \warning This function does \b NOT check if it is safe to render the objects and if all pointers are OK.
 * \note This function needs an \b active OpenGL context. Again there is no checking for one here!
 */
void rSceneBase::renderScene() {
   for( auto const & d : vObjects ) {
      if( d.vRenderer )
         d.vRenderer->render();
   }
}

/*!
 * \brief Adds a shader
 * \note This function only adds the shaders but does \b not compile them. Therefore it does not need an active OpenGL context
 *
 * Use compileShaders to compile all shaders.
 *
 * \todo Make a GLOBAL shader storage class
 *
 * \returns The index of the shader
 */
int rSceneBase::addShader( std::string _shader ) {
   std::lock_guard<std::mutex> lLockShaders( vShaders_MUT );

   vShaders.emplace_back( _shader );
   return vShaders.size() - 1;
}


/*!
 * \brief Adds an object to render
 *
 * \todo Implement functions to remove / disable / enable objects
 *
 * \note This function does not test if the shader exists
 *
 * \param[in] _obj         Pointer to an object
 * \param[in] _shaderIndex Shader to use
 *
 * \returns The Index of the object
 */
int rSceneBase::addObject( e_engine::rObjectBase *_obj, GLint _shaderIndex ) {
   std::lock_guard<std::mutex> lLockObjects( vObjects_MUT );

   vObjects.emplace_back( _obj, _shaderIndex );

   int lFlags;

   _obj->getHints( rObjectBase::FLAGS, lFlags );
   if( lFlags & AMBIENT_LIGHT || lFlags & LIGHT_SOURCE )
      vLightSourcesIndex.emplace_back( vObjects.size() - 1 );

   return vObjects.size() - 1;
}

/*!
 * \brief Compiles all shaders set with addShader
 *
 * \note This function will abort when a shader fails to compile
 *
 * \returns 1 on success or the error code of rShader::compile
 */
int rSceneBase::compileShaders() {
   std::lock_guard<std::mutex> lLockShaders( vShaders_MUT );

   int lRet = 1;
   for( auto & d : vShaders ) {
      if( d.getIsLinked() )
         continue;

      lRet = d.compile();
      if( lRet < 1 ) {
         eLOG( "Failed to compile shader '", d.getShaderPath(), "' Error code: ", lRet, " [SCENE: '", vName_str, "']" );
         return lRet;
      }
      dLOG( "Shader OK: '", d.getShaderPath(), "' [SCENE: '", vName_str, "']" );
   }
   return lRet;
}


/*!
 * \brief Sets the information the renderer needs
 *
 * \returns 0 on success
 */
int rSceneBase::assignObjectRenderer( GLuint _index, rRenderBase *_renderer ) {
   dLOG( "Setting rendering properties for object '", vObjects[_index].vObjectPointer->getName(), "'" );

   _renderer->setDataFromShader( &vShaders[vObjects[_index].vShaderIndex] );
   _renderer->setDataFromObject( vObjects[_index].vObjectPointer );

   int lLightModel;
   vObjects[_index].vObjectPointer->getHints( rObjectBase::LIGHT_MODEL, lLightModel );

   switch( lLightModel ) {
      case rObjectBase::SIMPLE_ADS_LIGHT: {
         for( auto i : vLightSourcesIndex ) {
            dLOG( "  - Using light source: '", vObjects[i].vObjectPointer->getName(), "'" );
            _renderer->setDataFromAdditionalObjects( vObjects[i].vObjectPointer );
         }
         break;
      }
      default: break;
   }

   if( vObjects[_index].vRenderer )
      delete vObjects[_index].vRenderer;

   vObjects[_index].vRenderer = _renderer;
   return 0;
}

/*!
 * \brief Assigns input locations to a type based on shader variable names
 *
 * \note This function must be run, before setObjectRenderer()
 *
 * See rShader::parseRawInformation for further information
 *
 * \returns The number of failed shaders (0 == everything fine)
 */
int rSceneBase::parseShaders() {
   std::lock_guard<std::mutex> lLockShaders( vShaders_MUT );

   int lErrors = 0;
   for( auto & d : vShaders ) {
      if( ! d.getIsLinked() ) {
         wLOG( "Shader '", d.getShaderPath(), "' is not linked" );
         ++lErrors;
         continue;
      }

      if( ! d.parseRawInformation() ) {
         wLOG( "Failed parsing shader '", d.getShaderPath(), "' [SCENE: '", vName_str, "']" );
         ++lErrors;
      }
   }
   return lErrors;
}




}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
