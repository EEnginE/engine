/// \fileshader.cpp
/// \brief \b Classes: \a eShader
/// \sa e_shader.hpp e_linker.hpp
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


#include "shader.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "log.hpp"

namespace e_engine {

/*
 * Konstruktor
 */
eShader::eShader( std::string _filename, GLenum _shadertype ) {
   filename    = _filename;
   data.clear();
   shadertype  = _shadertype;
   theShader   = 0;
   is_compiled = false;

   which_shader();
}


/** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **/

void eShader::which_shader() {
   switch ( shadertype ) {
      case GL_VERTEX_SHADER_ARB:   shadertype_str = "vertex";   break;
      case GL_FRAGMENT_SHADER_ARB: shadertype_str = "fragment"; break;
      case GL_GEOMETRY_SHADER_ARB: shadertype_str = "geometry"; break;
      default:                     shadertype_str = "unknown";  break;
   }
}


/*
 * Reads the source of the
 * shader to 'data'
 */
bool eShader::read_shader() {
   std::ifstream file;
   std::stringstream buffer;

   file.open( filename.c_str() );
   if ( !file.is_open() ) {
      std::string error = "Error at opening ";
      error.append( filename );
      error.append( " for reading" );
      return false;
   }

   //file >> data;
   buffer << file.rdbuf();
   data = buffer.str();
   buffer.clear();

   file.close();
   return true;
}

/*!
 * Checks for compiler errors
 *
 * \returns 1 When successful
 * \returns 4 When a shader compilation error occurs
 */
unsigned int eShader::test_shader() {
   int test;
   glGetShaderiv( theShader, GL_COMPILE_STATUS, &test );
   if ( test == GL_FALSE ) {
      char   log[5000];
      glGetShaderInfoLog( theShader, 5000, NULL, log );

      eLOG    "Compile failure in the " ADD shadertype_str ADD " shader " ADD filename ADD ':'
      NEWLINE "###################################################################################" NEWLINE
      NEWLINE log NEWLINE
      NEWLINE "###################################################################################" END
      glDeleteShader( theShader );

      is_compiled = false;
      std::string temp = "Error at compiling the ";
      temp += shadertype_str + " shader " + filename;

      //Returns a shader compilation error
      return 4;

   }
   return 1;
}

/*!
 * This runs read_shader, compiles
 * the shader and returns it
 *
 * \param _shader The shader that will be created
 * \returns 1 When successful
 * \returns 2 When a file-reading error occurs
 */
unsigned int eShader::compile( GLuint &_shader ) {
   data.clear();
   if ( read_shader() == false ) {
      eLOG "Error while reading source file '" ADD filename ADD "' (throw - E_CAN_NOT_READ_FILE)" END
      std::string temp = "Error while reading file ";
      temp += filename;
      //Return the file-reading error
      return 2;
   }            // reading...

   const GLcharARB *temp = data.c_str();
   // Create Shader
   theShader = glCreateShader( shadertype );
   // Adding source
   glShaderSource( theShader, 1, &temp, NULL );
   // Compiling
   glCompileShader( theShader );
   data.clear();


   is_compiled = true;
   _shader =  theShader;
   return test_shader();
}
}
