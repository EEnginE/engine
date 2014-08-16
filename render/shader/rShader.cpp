/*!
 * \file rShader.cpp
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


#include <GL/glew.h>
#include "rShader.hpp"
#include <cstdarg>
#include <boost/filesystem.hpp>
#include "uLog.hpp"
#include "defines.hpp"
#include <stdio.h>


namespace e_engine {

rShader::rShader() {
   vPath_str             = "";
   vShaderProgram_OGL    = 0;
   vIsShaderLinked_B     = false;

   vShaderEndings[VERT]  = VERT_END;
   vShaderEndings[FRAG]  = FRAG_END;
   vShaderEndings[GEOM]  = GEOM_END;
}


rShader::rShader( std::string _path ) {
   vPath_str     = _path;
   vShaderProgram_OGL    = 0;
   vIsShaderLinked_B     = false;

   vShaderEndings[VERT]  = VERT_END;
   vShaderEndings[FRAG]  = FRAG_END;
   vShaderEndings[GEOM]  = GEOM_END;
}

rShader::rShader( std::string _path, GLuint n, ... ) {
   vPath_str             = _path;
   vShaderProgram_OGL    = 0;
   vIsShaderLinked_B     = false;

   vShaderEndings[VERT]  = VERT_END;
   vShaderEndings[FRAG]  = FRAG_END;
   vShaderEndings[GEOM]  = GEOM_END;

   vLinkerAttributes.clear();
   va_list list;
   va_start( list, n );
   for( unsigned int i = 0; i < n; i++ ) {
      e_engine_internal::atributeObject attribute;
      attribute.index = va_arg( list, int );
      attribute.name  = va_arg( list, char * );
      vLinkerAttributes.push_back( attribute );
   }
   va_end( list );
}

// =========================================================================================================================
// ==============================================================================================================================================
// =========                =================================================================================================================================
// =======   Link the shader  ====================================================================================================================================
// =========                =================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

/*!
 * \brief Search for shader files
 * \returns The number of found shader files
 */
int rShader::search_shaders() {
   std::string temp[] = {vPath_str, vPath_str, vPath_str};

   temp[VERT] += vShaderEndings[VERT];
   temp[FRAG] += vShaderEndings[FRAG];
   temp[GEOM] += vShaderEndings[GEOM];

   boost::filesystem::path vert( temp[VERT] );
   boost::filesystem::path frag( temp[FRAG] );
   boost::filesystem::path geom( temp[GEOM] );

   try {
      if( boost::filesystem::exists( vert ) )     // Look for the vertex-shader
         if( boost::filesystem::is_regular_file( vert ) )
            vShaders.emplace_back( temp[VERT], GL_VERTEX_SHADER );

      if( boost::filesystem::exists( frag ) )     // Look for the fragment-shader
         if( boost::filesystem::is_regular_file( vert ) )
            vShaders.emplace_back( temp[FRAG], GL_FRAGMENT_SHADER );

      if( boost::filesystem::exists( geom ) )     // Look for the geometry-shader
         if( boost::filesystem::is_regular_file( geom ) )
            vShaders.emplace_back( temp[GEOM], GL_GEOMETRY_SHADER );


   } catch( const boost::filesystem::filesystem_error &ex ) {
      eLOG ex.what() END
   } catch( ... ) {
      eLOG "Caught unknown exception" END
   }

   if( vShaders.size() == 0 ) {
      wLOG "No shaders found! WARNING!" END
   }

   return vShaders.size();
}


bool rShader::addShader( std::string _filename, GLenum _shaderType ) {
   boost::filesystem::path lShaderPath( _filename );

   try {
      if( boost::filesystem::exists( lShaderPath ) ) {     // Look for the vertex-shader
         if( boost::filesystem::is_regular_file( lShaderPath ) ) {

            switch( _shaderType ) {
               case GL_VERTEX_SHADER:   vShaders.emplace_back( _filename, GL_VERTEX_SHADER );   return true;
               case GL_FRAGMENT_SHADER: vShaders.emplace_back( _filename, GL_FRAGMENT_SHADER ); return true;
               case GL_GEOMETRY_SHADER: vShaders.emplace_back( _filename, GL_GEOMETRY_SHADER ); return true;
               default:
                  eLOG "Unknown shader type " ADD _shaderType ADD " of " ADD _filename END
                  return false;
            }
         }
      }
   } catch( const boost::filesystem::filesystem_error &ex ) {
      eLOG ex.what() END
   } catch( ... ) {
      eLOG "Caught unknown exception" END
   }

   eLOG "Failed to access shader file " ADD _filename END
   return false;
}


/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/*!
 * \brief Test if program is OK
 * \returns 1 When successful
 * \returns -5 When a shader linking error occured
 */
unsigned int rShader::testProgram() {
   int status;
   glGetProgramiv( vShaderProgram_OGL, GL_LINK_STATUS, &status );
   if( status == GL_FALSE ) {
      char log[5000];
      glGetProgramInfoLog( vShaderProgram_OGL, 5000, NULL, log );

      eLOG    "Linking failure:"
      NEWLINE "###################################################################################" NEWLINE
      NEWLINE log NEWLINE
      NEWLINE "###################################################################################" END
      glDeleteProgram( vShaderProgram_OGL );

      vIsShaderLinked_B = false;

      //Return a shader linking error
      return -5;
   }
   return 1;
}


/*!
 *
 * \brief Search the shader files, compile them and link the program
 *
 * \returns the number of the (successfully) compiled and linked shader files
 * \returns -2 When a file reading error occurs
 * \returns -3 When a file not found error occurs
 * \returns -4 When a shader compilation error occurs
 * \returns -5 When a shader linking error occurs
 * \returns -6 When the shaders were not set
 */
int rShader::compile( GLuint &_vShader_OGL ) {
   unsigned int lRet_uI = compile();
   _vShader_OGL = vShaderProgram_OGL;
   return lRet_uI;
}

/*!
 *
 * \brief Search the shader files, compile them and link the program
 *
 * \returns the number of the (successfully) compiled and linked shader files
 * \returns -2 When a file reading error occurs
 * \returns -3 When a file not found error occurs
 * \returns -4 When a shader compilation error occurs
 * \returns -5 When a shader linking error occurs
 * \returns -6 When the shaders were not set
 */
int rShader::compile() {
   if( vPath_str.empty() && vShaders.empty() ) {
      eLOG "No shaders set for compilation" END
      return -6;
   }

   if( search_shaders() == 0  && vShaders.empty() ) {
      eLOG "Unable to find any shader file ( Path: " ADD vPath_str ADD " )" END
      //Return a file not found error
      return -3;
   }

   for( auto & s : vShaders ) {
      // Compile shaders
      //Check for errors
      if( s.compileShader() != 1 )
         return -4;
   }


   // Createng the program
   vShaderProgram_OGL = glCreateProgram();

   // Adding shaders
   for( auto & s : vShaders ) {
      glAttachShader( vShaderProgram_OGL, s.vShader_OGL );
   }

   // Binding Attributes
   if( vLinkerAttributes.empty() == false ) {
      for( GLuint i = 0; i < vLinkerAttributes.size(); i++ ) {
         glBindAttribLocation( vShaderProgram_OGL, vLinkerAttributes[i].index, vLinkerAttributes[i].name.c_str() );
      }
   }

   // Linking
   glLinkProgram( vShaderProgram_OGL );

   // Delete old shaders. Not needed anymore
   for( auto & s : vShaders ) {
      glDeleteShader( s.vShader_OGL );
   }

   // Linking successful? Returns a shader linking error if unsuccessful
   if( testProgram() != 1 )
      return -5;


   // Output
   LOG_ENTRY lEntry_LOG = iLOG "Program with the " ADD vShaders.size() ADD " shader(s) successfully linked" _END_
   for( auto & s : vShaders ) {
      lEntry_LOG _POINT s.vFilename_str _END_
   }
   lEntry_LOG __END__
   vIsShaderLinked_B = true;

   int lTempShaderCounter_I = vShaders.size();
   vShaders.clear();
   getProgramInfo();

   return lTempShaderCounter_I;
}


/*!
 * \brief Deletes the programm
 * \returns Nothing
 */
void rShader::deleteProgram() {
   if( !vIsShaderLinked_B )
      return; // Don't delete twice
   glDeleteProgram( vShaderProgram_OGL );
   vIsShaderLinked_B = false;
}

// =========================================================================================================================
// ==============================================================================================================================================
// =========                    =============================================================================================================================
// =======   Compile the Shaders  ================================================================================================================================
// =========                    =============================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

std::string getShaderTypeString( GLenum _type ) {
   switch( _type ) {
      case GL_VERTEX_SHADER_ARB:   return "vertex";
      case GL_FRAGMENT_SHADER_ARB: return "fragment";
      case GL_GEOMETRY_SHADER_ARB: return "geometry";
      default:                     return "unknown";
   }
}

bool rShader::singleShader::readShader() {
   FILE *lFile = fopen( vFilename_str.c_str(), "r" );
   if( lFile == NULL ) {
      eLOG "Unable to open " ADD vFilename_str END
      return false;
   }

   int c;
   vData_str.clear();

   while( ( c = fgetc( lFile ) ) != EOF )
      vData_str.append( 1, ( char )c );

   fclose( lFile );

   return true;
}

/*!
 * Checks for compiler errors
 *
 * \returns true  When successful
 * \returns false When a shader compilation error occurs
 */
bool rShader::singleShader::testShader() {
   int test;
   glGetShaderiv( vShader_OGL, GL_COMPILE_STATUS, &test );
   if( test == GL_FALSE ) {
      char   log[5000];
      glGetShaderInfoLog( vShader_OGL, 5000, NULL, log );

      eLOG    "Compile failure in the " ADD getShaderTypeString( vShader_OGL ) ADD " shader " ADD vFilename_str ADD ':'
      NEWLINE "###################################################################################" NEWLINE
      NEWLINE log NEWLINE
      NEWLINE "###################################################################################" END
      glDeleteShader( vShader_OGL );

      //Returns a shader compilation error
      return false;

   }
   return true;
}

/*!
 * This runs read_shader, compiles
 * the shader and returns it
 *
 * \param _shader The shader that will be created
 * \returns 1 When successful
 * \returns 2 When a file-reading error occurs
 * \returns 3 When a compile error occurs
 */
int rShader::singleShader::compileShader() {
   vData_str.clear();
   if( readShader() == false ) {
      eLOG "Error while reading source file '" ADD vFilename_str ADD "'" END
      //Return the file-reading error
      return 2;
   }

   const GLcharARB *temp = vData_str.c_str();
   // Create Shader
   vShader_OGL = glCreateShader( vShaderType );
   // Adding source
   glShaderSource( vShader_OGL, 1, &temp, NULL );
   // Compiling
   glCompileShader( vShader_OGL );
   vData_str.clear();


   return testShader() == true ? 1 : 3;
}







/*!
 * \brief Get the linked program
 * \param[out] _program The linked shader program
 * \returns true if the program was linked and false if not
 */
bool rShader::getProgram( unsigned int &_program ) const  {
   if( !vIsShaderLinked_B )
      return false;
   _program = vShaderProgram_OGL;
   return true;
}



/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */


/*!
 * \brief Set additional attributes
 * \param[in] n Number of following attribute pairs
 *
 * Adds attributes to the program when its linked.
 *
 * USE: <number of atrributes> , attributes: [int index, char* name]
 *
 * \returns Nothing
 */
void rShader::setAttributes( unsigned int n, ... ) {
   vLinkerAttributes.clear();
   va_list list;
   va_start( list, n );
   for( unsigned int i = 0; i < n; i++ ) {
      e_engine_internal::atributeObject attribute;
      attribute.index = va_arg( list, int );
      attribute.name  = va_arg( list, char * );
      vLinkerAttributes.push_back( attribute );
   }
   va_end( list );
}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

