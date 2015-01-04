/*!
 * \file rShader.hpp
 * \brief \b Classes: \a rShader
 * \sa rShader
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


#ifndef LINKER_HPP
#define LINKER_HPP

#include "defines.hpp"

#include <vector>
#include <string>
#include "rShader_structs.hpp"


namespace e_engine {

static const std::string VERT_END( ".vert" ); //!< Standard shader ending for vertex shaders
static const std::string FRAG_END( ".frag" ); //!< Standard shader ending for fragment shaders
static const std::string GEOM_END( ".geom" ); //!< Standard shader ending for geometry shaders



/*!
 * \class e_engine::rShader
 * \brief Class to link a \c GLSL program
 *
 * This class can find \c GLSL files and link them
 * to a \c GLSL program
 *
 */
class rShader {
 public:
   enum SHADER_TYPE { VERT = 0, FRAG, GEOM };

   enum SHADER_INFORMATION {
      VERTEX_INPUT = 0,
      NORMALS_INPUT,
      __BEGIN_UNIFORMS__,

      // Matrices
      MODEL_MATRIX,
      VIEW_MATRIX,
      MODEL_VIEW_MATRIX,
      PROJECTOIN_MATRIX,
      M_V_P_MATRIX,
      NORMAL_MATRIX,

      // Light
      AMBIENT_COLOR,
      NUM_LIGHTS,
      LIGHT_COLOR,
      LIGHT_POSITION,

      __END_INF__
   };

 private:
   struct singleShader {
      std::string vFilename_str;
      GLenum vShaderType;
      std::string vData_str;
      GLuint vShader_OGL;

      bool readShader();
      bool testShader();
      int compileShader();


      singleShader( std::string _file, GLenum _type )
          : vFilename_str( _file ), vShaderType( _type ) {}
   };

   std::vector<singleShader> vShaders;
   std::string vPath_str;

   std::string vShaderEndings[3];

   GLuint vShaderProgram_OGL;

   bool vIsShaderLinked_B;

   internal::programInfo vProgramInformation;
   bool vHasProgramInformation_B;

   unsigned int testProgram();
   void getProgramInfo();
   std::string
   processData( GLenum _type, GLuint _index, GLsizei _arraySize, GLenum *_in, GLint *_out );

   void getInfoOld();
   void getInfoNew();

   std::string getTypeString( int _type );

   struct {
      std::vector<GLint> locations; //!< locations (vector because of uniform arrays)
      std::string uName;            //!< Uniform name
      std::string sName;            //!< Struct name
      GLint type;
   } vInfo[__END_INF__];

 public:
   rShader();
   rShader( std::string _path ) : rShader() { vPath_str = _path; }
   ~rShader() {
      if ( vIsShaderLinked_B )
         deleteProgram();
   }

   // Forbid copying
   rShader( const rShader & ) = delete;
   rShader &operator=( const rShader & ) = delete;

   // Allow moving
   rShader( rShader &&_s );

   GLvoid setShaders( std::string _path ) { vPath_str = _path; }

   GLvoid setEndings( std::string _vert, std::string _frag, std::string _geom ) {
      vShaderEndings[VERT] = _vert;
      vShaderEndings[FRAG] = _frag;
      vShaderEndings[GEOM] = _geom;
   }

   bool addShader( std::string _filename, GLenum _shaderType );

   int search_shaders();

   int compile( GLuint &_vShader_OGL );
   int compile();

   void deleteProgram();
   bool getIsLinked() const { return vIsShaderLinked_B; }
   std::string getShaderPath() const { return vPath_str; }
   bool getProgram( unsigned int &_program ) const;

   internal::programInfo *getShaderInfo() { return &vProgramInformation; }

   bool parseRawInformation();

   void setUniformTypeString( SHADER_INFORMATION _type, std::string _str );
   void setUniformStructString( SHADER_INFORMATION _type, std::string _str );

   unsigned int getUniformArraySize( SHADER_INFORMATION _type ) const;

   GLint getLocation( SHADER_INFORMATION _type, unsigned int _index = 0 ) const;
};

/*!
 * \fn rShader::clearAttributes
 * \brief Clear the attribute list
 * \returns Nothing
 */

/*!
 * \fn rShader::getIsLinked
 * \brief Get if the  program is linked
 * \returns Whether the  program is linked or not
 */

/*!
 * \fn rShader::getShaderPath
 * \brief Get the main path of the shader(s)
 * \returns the main path of the shader(s)
 */

/*!
 * \fn rShader::getShaderFound
 * \brief Get the number of found shader files
 * \returns The number of found shader files
 */

/*!
 * \fn rShader::setShaders
 * \brief Set a new shader path
 * \param[in] _path Basic shader path
 * \returns Nothing
 */

/*!
 * \fn rShader::setEndings
 * \brief Set the standard endings to find shaders automatically.
 * \param _vert vertex-shader ending
 * \param _frag fragment-shader ending
 * \param _geom geometry-shader ending
 * \returns Nothing
 */

/*!
 * \fn rShader::getLocation
 *
 * You should run parseRawInformation() first
 *
 * \brief Returns the location of a shader input / uniform
 * \param[in] _type The type of the input / uniform
 * \returns The location or a negative value
 */
}

#endif // LINKER_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
