/*!
 * \file rShader.hpp
 * \brief \b Classes: \a rShader
 * \sa rLinker
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


#ifndef E_SHADER_HPP
#define E_SHADER_HPP
#include <string>
#include <GL/glew.h>

namespace e_engine {

/*!
 * \class e_engine::rShader
 * \brief Class for compiling \c GLSL shaders
 */
class rShader {
   private:

      std::string filename;
      std::string data;
      GLenum      shadertype;
      std::string shadertype_str;
      unsigned int      thrShader;

      bool        is_compiled;

      bool read_shader();
      unsigned int test_shader();
      void which_shader();

      rShader();

   public:
      /*!
       * \brief The constructor
       * \param _filename   The new filename
       * \param _shadertype The type of the shader
       */
      rShader( std::string _filename, GLenum _shadertype );
      ~rShader() {deletrShader();}

      /*!
       * \brief Set the filename to _filename
       * \param _filename   The new filename
       * \param _shadertype The type of the shader
       * \returns Nothing
       */
      void setFile( std::string _filename, GLenum _shadertype ) {
         filename   = _filename; shadertype = _shadertype;
         which_shader();
      }


      unsigned int compile( GLuint &_shader );

      //! Deletes the shader \returns Nothing
      void deletrShader() {
         if ( is_compiled ) {
            glDeleteShader( thrShader );
            is_compiled = false;
         }
      }

      std::string getData()             {return data;}             //!< Get the data           \returns The data
      std::string getFilename()         {return filename;}         //!< Get the filename       \returns The filename
      std::string getShaderTypeString() {return shadertype_str;}   //!< Get the shader type    \returns The shader type
      GLuint      getShader()           {return thrShader;}        //!< Get the shader         \returns The shader
      bool        getIsCompiled()       {return is_compiled;}      //!< Get if is compiled     \returns Whether is compiled
};

}

#endif // E_SHADER_HPP
