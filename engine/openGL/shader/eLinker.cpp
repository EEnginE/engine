/*!
 * \file eLinker.cpp
 * \brief \b Classes: \a eLinker
 * \sa eShader
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


#include "eLinker.hpp"
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "uLog.hpp"
#include <GL/glew.h>

namespace e_engine {

using namespace e_engine_internal;

eLinker::eLinker( std::string _path ) {
   path         = _path;
   found_shaders = 0;
   theProgram   = 0;
   is_linked    = false;

   ending[VERT] = VERT_END;
   ending[FRAG] = FRAG_END;
   ending[GEOM] = GEOM_END;
}

eLinker::eLinker( std::string _path, GLuint n, ... ) {
   path         = _path;
   found_shaders = 0;
   theProgram   = 0;
   is_linked    = false;

   ending[VERT] = VERT_END;
   ending[FRAG] = FRAG_END;
   ending[GEOM] = GEOM_END;

   attributes.clear();
   va_list list;
   va_start( list, n );
   for ( unsigned int i = 0; i < n; i++ ) {
      atributeObject attribute;
      attribute.index = va_arg( list, int );
      attribute.name  = va_arg( list, char * );
      attributes.push_back( attribute );
   }
   va_end( list );
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

int eLinker::search_shaders() {
   std::string temp[3] = {path, path, path};

   temp[VERT] += ending[VERT];
   temp[FRAG] += ending[FRAG];
   temp[GEOM] += ending[GEOM];

   boost::filesystem::path vert( temp[VERT] );
   boost::filesystem::path frag( temp[FRAG] );
   boost::filesystem::path geom( temp[GEOM] );

   try {
      if ( boost::filesystem::exists( vert ) ) {    // Look for the vertex-shader
         if ( boost::filesystem::is_regular_file( vert ) ) {
            eShader s( temp[VERT], GL_VERTEX_SHADER );
            shaders.push_back( s );
            found_shaders++;
         }
      }
      if ( boost::filesystem::exists( frag ) ) {    // Look for the fragment-shader
         if ( boost::filesystem::is_regular_file( vert ) ) {
            eShader s( temp[FRAG], GL_FRAGMENT_SHADER );
            shaders.push_back( s );
            found_shaders++;
         }
      }
      if ( boost::filesystem::exists( geom ) ) {    // Look for the geometry-shader
         if ( boost::filesystem::is_regular_file( geom ) ) {
            eShader s( temp[GEOM], GL_GEOMETRY_SHADER );
            shaders.push_back( s );
            found_shaders++;
         }
      }
   } catch ( const boost::filesystem::filesystem_error &ex ) {
      eLOG ex.what() END
   } catch ( ... ) {
      eLOG "Caught unknown exception" END
   }

   if ( found_shaders == 0 ) {
      wLOG "Found 0 shader(s) WARNING!!!" END
   }

   return found_shaders;
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/*!
       * \brief Test if program is OK
       * \returns 1 When successful
       * \returns 5 When a shader linking error occured
       */
unsigned int eLinker::test_program() {
   int status;
   glGetProgramiv( theProgram, GL_LINK_STATUS, &status );
   if ( status == GL_FALSE ) {
      char log[5000];
      glGetProgramInfoLog( theProgram, 5000, NULL, log );

      eLOG    "Linking failure:"
      NEWLINE "###################################################################################" NEWLINE
      NEWLINE log NEWLINE
      NEWLINE "###################################################################################" END
      glDeleteProgram( theProgram );

      is_linked = false;

      //Return a shader linking error
      return 5;
   }
   return 1;
}


/*!
 *
 * \brief Search the shader files, compile them and link the program
 *
 * \param _theProgram The Program that will be created
 * \returns 1 When successful
 * \returns 2 When a file reading error occurs
 * \returns 3 When a file not found error occurs
 * \returns 4 When a shader compilation error occurs
 * \returns 5 When a shader linking error occurs
 */
unsigned int eLinker::link( GLuint &_theProgram ) {
   unsigned int i;

   if ( search_shaders() == 0 ) {
      eLOG "Unable to find any shader file (throw - E_NOT_SUCH_A_FILE)" END
      //Return a file not found error
      return 3;
   }

   for ( i = 0; i < shaders.size(); i++ ) {
      /// Compile shaders
      //Check for errors
      GLuint dummy;
      int errorCheck = shaders[i].compile( dummy );
      if ( errorCheck != 1 ) {
         if ( errorCheck == 2 ) {
            eLOG "Failed to read file." END
         } else if ( errorCheck == 4 ) {
            eLOG "Failed to compile shader." END
         }
         //Return a shader linking error
         return 5;
      }
   }


   /// Creating the program
   theProgram = glCreateProgram();

   /// Adding shaders
   for ( GLuint i = 0; i < shaders.size(); i++ ) {
      glAttachShader( theProgram, shaders[i].getShader() );
   }

   /// Binding Attributes
   if ( attributes.empty() == false ) {
      for ( GLuint i = 0; i < attributes.size(); i++ ) {
         glBindAttribLocation( theProgram, attributes[i].index, attributes[i].name.c_str() );
      }
   }

   /// Linking
   glLinkProgram( theProgram );

   /// Delete old shaders
   for ( GLuint i = 0; i < shaders.size(); i++ ) {
      shaders[i].deleteShader();
   }

   /// Linking successful? Returns a shader linking error if unsuccessful
   if ( test_program() != 1 )
      return 5;


   /// Output
   LOG_ENTRY lEntry_LOG = iLOG "Program with the " ADD found_shaders ADD " shader(s) successfully linked" _END_
   for ( unsigned int ii = 0; ii < found_shaders; ii++ ) {
      lEntry_LOG _POINT shaders[ii].getFilename() _END_
   }
   lEntry_LOG _END
   is_linked = true;
   _theProgram = theProgram;
   return 1;
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

/*
 * Adds attributes to the program when its linked.
 *
 * USE: <number of atrributes> , attributes: [int index, char* name]
 */
void eLinker::setAttributes( unsigned int n, ... ) {
   attributes.clear();
   va_list list;
   va_start( list, n );
   for ( unsigned int i = 0; i < n; i++ ) {
      atributeObject attribute;
      attribute.index = va_arg( list, int );
      attribute.name  = va_arg( list, char * );
      attributes.push_back( attribute );
   }
   va_end( list );
}

}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
