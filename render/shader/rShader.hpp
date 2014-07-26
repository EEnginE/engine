/*!
 * \file rShader.hpp
 * \brief \b Classes: \a rShader
 * \sa rShader
 */
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef LINKER_HPP
#define LINKER_HPP

#include <vector>
#include <string>
#include "rShader_structs.hpp"

namespace e_engine {

static const std::string VERT_END( ".vert" );   //!< Standard shader ending for vertex shaders
static const std::string FRAG_END( ".frag" );   //!< Standard shader ending for fragment shaders
static const std::string GEOM_END( ".geom" );   //!< Standard shader ending for geometry shaders



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
      enum SHADER_TYPE {
         VERT = 0,
         FRAG,
         GEOM
      };

   private:
      struct singleShader {
         std::string vFilename_str;
         GLenum      vShaderType;
         std::string vData_str;
         GLuint      vShader_OGL;

         bool readShader();
         bool testShader();
         int  compileShader();


         singleShader( std::string _file, GLenum _type ) : vFilename_str( _file ), vShaderType( _type ) {}
      };

      std::vector<singleShader> vShaders;
      std::string               vPath_str;
      std::vector<e_engine_internal::atributeObject> vLinkerAttributes;

      std::string                    vShaderEndings[3];

      GLuint                         vShaderProgram_OGL;

      bool                           vIsShaderLinked_B;

      e_engine_internal::programInfo vProgramInformation;
      bool                           vHasProgramInformation_B;

      unsigned int testProgram();
      void         getProgramInfo();
      std::string  processData( GLenum _type, GLuint _index, GLsizei _arraySize, GLenum *_in, GLint *_out );

   public:

      rShader();
      rShader( std::string _path );
      rShader( std::string _path, GLuint n, ... );
      ~rShader() { if( vIsShaderLinked_B ) deleteProgram();}

      GLvoid   setShaders( std::string _path ) { vPath_str = _path; }

      GLvoid   setEndings( std::string _vert, std::string _frag, std::string _geom ) {
         vShaderEndings[VERT] = _vert;
         vShaderEndings[FRAG] = _frag;
         vShaderEndings[GEOM] = _geom;
      }

      bool   addShader( std::string _filename, GLenum _shaderType );


      void   setAttributes( unsigned int n, ... );
      void   clearAttributes() {vLinkerAttributes.clear();}

      int    search_shaders();

      int compile( GLuint &_vShader_OGL );
      int compile();

      void         deleteProgram();
      bool         getIsLinked()    const {return vIsShaderLinked_B;}
      std::string  getShaderPath()  const {return vPath_str;}
      bool         getProgram( unsigned int &_program ) const;

      e_engine_internal::programInfo getShaderInfo() { return vProgramInformation; }

      bool         getInputLocation( std::string _name, int &_location );
      bool         getOutputLocation( std::string _name, int &_location );
      bool         getUniformLocation( std::string _name, int &_location );
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

}

#endif // LINKER_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
