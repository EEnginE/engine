/*!
 * \file rNormalObject.hpp
 * \brief \b Classes: \a rNormalObject
 */

#ifndef R_NORMAL_OBJECT_HPP
#define R_NORMAL_OBJECT_HPP

#include <vector>
#include <string>
#include "rRenderNormalOBJBase.hpp"
#include "rShader.hpp"
#include "iInit.hpp"
#include "rLoader_3D_f_OBJ.hpp"
#include "rMatrixObjectBase.hpp"
#include "rWorld.hpp"

namespace e_engine {

class rNormalObject : public rMatrixObjectBase<float> {
      typedef e_engine_internal::rRenderNormalOBJBase<float> RENDERER;
      typedef e_engine_internal::programInfo                 S_INF;
      enum DATA_FILE_TYPE { AUTODETECT, OBJ_FILE };
   private:
      struct dataFile {
         std::string path;
         DATA_FILE_TYPE type;

         dataFile( std::string _p, DATA_FILE_TYPE _t ) : path( _p ), type( _t ) {}
      };

      RENDERER             *vRenderer;

      std::vector<GLuint>   vVertexBufferObjects;
      std::vector<GLuint>   vIndexBufferObjects;

      bool                  vHasGeneretedBuffers_B;
      bool                  vIsDataLoaded_B;

      std::string           vObjectName;

      std::vector<dataFile> vDataFiles;
      std::vector<rShader>  vShaders;

      std::vector<S_INF>    vShaderInfo;
      std::vector<GLint>    vInputLocations;
      std::vector<GLint>    vUniformLocations;
      std::vector<GLuint>   vNumOfIndexes;

      bool           vNeedUpdateMatrix_B;

      DATA_FILE_TYPE detectFileTypeFromEnding( std::string const &_str );
      RENDERER_ID    chooseRendere();

      rNormalObject() {}
   public:
      rNormalObject( std::string _name );
      virtual ~rNormalObject();

      inline void render()     {vRenderer->render();}
      inline void operator()() {vRenderer->render();}

      int  loadData( e_engine::rWorld *_world );
      void freeData();

      bool addShader( std::string _shaderPath );
      bool addData( std::string _pathToDataFile, e_engine::rNormalObject::DATA_FILE_TYPE _type = AUTODETECT );

      std::vector<std::string> getDataFileNames();
      std::vector< rShader > *getShaders();

      inline bool getCanRender() const {return vIsDataLoaded_B;}

      inline void updateUniforms()                   {if( vRenderer ) vRenderer->updateUniforms();}
      inline void updateUniformsAlways( bool _doit ) {if( vRenderer ) vRenderer->updateUniformsAlways( _doit );}

      void reset();
};

/*!
 * \fn rNormalObject::render
 *
 * \brief renders the data stored in the object
 *
 * \warning Executing this funktion without loading data WILL CAUSE A \b SEGFAULT!!!
 *
 * \note if getCanRender() is returns true, you can savely use this function
 *
 * This function does NOT check if the data is loaded and everything is ready for rendering
 * to provide maximal performance!
 *
 * If you are not sure if you can use this function, check the output of getCanRender()
 */

/*!
 * \fn rNormalObject::operator()
 *
 * \brief renders the data stored in the object
 *
 * \warning Executing this funktion without loading data WILL CAUSE A \b SEGFAULT!!!
 *
 * \note if getCanRender() is returns true, you can savely use this function
 *
 * This function does NOT check if the data is loaded and everything is ready for rendering
 * to provide maximal performance!
 *
 * If you are not sure if you can use this function, check the output of getCanRender()
 */

/*!
 * \fn rNormalObject::getCanRender
 * \brief Checks if it is possible to render the object
 * \returns if it is possible to render the object
 */

}

#endif // R_NORMAL_OBJECT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
