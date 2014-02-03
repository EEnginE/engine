/// \fileshader.hpp 
/// \brief \b Classes: \a eShader
/// \sa e_shader.cpp e_linker.hpp
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

#include "error.hpp"
#include <string>

namespace e_engine {

/*!
 * \class e_engine::eShader
 * \brief Class for compiling \c GLSL shaders
 */
class eShader {
   private:

      std::string filename;
      std::string data;
      GLenum      shadertype;
      std::string shadertype_str;
      GLuint      theShader;

      bool        is_compiled;

      bool read_shader();
     GLvoid test_shader();
     GLvoid which_shader();

      eShader();

   public:
      /*!
       * \brief The constructor
       * \param _filename   The new filename
       * \param _shadertype The type of the shader
       */
      eShader( std::string _filename, GLenum _shadertype );
      ~eShader() {deleteShader();}

      /*!
       * \brief Set the filename to _filename
       * \param _filename   The new filename
       * \param _shadertype The type of the shader
       * \returns Nothing
       */
     GLvoid setFile( std::string _filename, GLenum _shadertype ) {
         filename   = _filename; shadertype = _shadertype;
         which_shader();
      }

      /*!
       * \brief Compile the shader
       * 
       * This runs read_shader and compile
       * the shader and returns it
       *
       * \throws eError When there is an error
       * \returns The compiled shader
       */
      GLuint compile();

      //! Deletes the shader \returns Nothing
     GLvoid deleteShader() {
         if ( is_compiled ) {
            glDeleteShader( theShader );
            is_compiled = false;
         }
      }

      std::string getData()             {return data;}             //!< Get the data           \returns The data
      std::string getFilename()         {return filename;}         //!< Get the filename       \returns The filename
      std::string getShaderTypeString() {return shadertype_str;}   //!< Get the shader type    \returns The shader type
      GLuint      getShader()           {return theShader;}        //!< Get the shader         \returns The shader
      bool        getIsCompiled()       {return is_compiled;}      //!< Get if is compiled     \returns Whether is compiled
};

}

#endif // E_SHADER_HPP
