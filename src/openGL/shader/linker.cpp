/// \filelinker.cpp
/// \brief \b Classes: \a eLinker
/// \sa e_linker.cpp e_shader.hpp
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


#include "linker.hpp"
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "platform.hpp"
#include "log.hpp"

namespace e_engine {

using namespace e_engine_internal;

eLinker::eLinker( std::string _path ) {
   path         = _path;
   found_shadrs = 0;
   theProgram   = 0;
   is_linked    = false;

   ending[VERT] = VERT_END;
   ending[FRAG] = FRAG_END;
   ending[GEOM] = GEOM_END;
}

eLinker::eLinker( std::string _path, GLuint n, ... ) {
   path         = _path;
   found_shadrs = 0;
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
      attribute.name  = va_arg( list, char* );
      attributes.push_back( attribute );
   }
   va_end( list );
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

int eLinker::serch_shaders() {
   std::string temp[3] = {path, path, path};

   temp[VERT] += ending[VERT];
   temp[FRAG] += ending[FRAG];
   temp[GEOM] += ending[GEOM];

   boost::filesystem::path vert( temp[VERT] );
   boost::filesystem::path frag( temp[FRAG] );
   boost::filesystem::path geom( temp[GEOM] );

   try {
      if ( boost::filesystem::exists( vert ) ) {    // Look for the vertex-shadr
         if ( boost::filesystem::is_regular_file( vert ) ) {
            eShader s( temp[VERT], GL_VERTEX_SHADER );
            shaders.push_back( s );
            found_shadrs++;
         }
      }
      if ( boost::filesystem::exists( frag ) ) {    // Look for the fragment-shadr
         if ( boost::filesystem::is_regular_file( vert ) ) {
            eShader s( temp[FRAG], GL_FRAGMENT_SHADER );
            shaders.push_back( s );
            found_shadrs++;
         }
      }
      if ( boost::filesystem::exists( geom ) ) {    // Look for the geometry-shadr
         if ( boost::filesystem::is_regular_file( geom ) ) {
            eShader s( temp[GEOM], GL_GEOMETRY_SHADER );
            shaders.push_back( s );
            found_shadrs++;
         }
      }
   } catch ( const boost::filesystem::filesystem_error& ex ) {
      eLOG ex.what() END
   } catch ( ... ) {
      eLOG "Caught unknown exeption" END
   }

   if ( found_shadrs == 0 ) {
      wLOG "Found 0 shader(s) WARNING!!!" END
   }

   return found_shadrs;
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

void eLinker::test_program() {
   GLint status;
   glGetProgramiv( theProgram, GL_LINK_STATUS, &status );
   if ( status == GL_FALSE ) {
      GLchar log[5000];
      glGetProgramInfoLog( theProgram, 5000, NULL, log );

      eLOG    "Linking failure:" 
      NEWLINE "###################################################################################" NEWLINE
      NEWLINE log NEWLINE
      NEWLINE "###################################################################################" END
      glDeleteProgram( theProgram );

      is_linked = false;

      eError e( CLASS_eShader, ERROR_Shader, E_LINKING_FAILURE, "Error at linking the shaders" );
      throw e;
   }
}


/*
 * Try to link the program with the compiled shaders and removes them.
 */
GLuint eLinker::link() {
   GLuint i;

   if ( serch_shaders() == 0 ) {
      eLOG "Unable to find any shader file (throw - E_NOT_SUCH_A_FILE)" END
      eError e( CLASS_eLinker, ERROR_InputOutput, E_NOT_SUCH_A_FILE, "Unable to find any shader file" );
      throw e;
   }

   try { // --- try
      for ( i = 0; i < shaders.size(); i++ ) {
         /// Compile shaders
         shaders[i].compile();
      }
   } catch ( eError &e ) { // --- catch

      // from eShader?
      if ( e.getClassID() != CLASS_eShader && e.getErrorID() != ERROR_Shader ) {e.what();}

      eLOG "Failed to link program. Abort. (throw - E_COMPILING_FAILURE)" END
      eError eee( CLASS_eLinker, ERROR_Shader, E_COMPILING_FAILURE, "Failed to link Programm" );
      eee.addError( e );
      throw eee;

   } catch ( ... ) {
      wLOG "Received unknown exception! Continuing!" END
   } // --- END catch



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

   /// Linking successfull?
   test_program();

   /// Output
   LOG_ENTRY lEntry_LOG = iLOG "Programm with the " ADD found_shadrs ADD " shader(s) successfully linked" _END_
   for ( unsigned int ii = 0; ii < found_shadrs; ii++ ) {
      lEntry_LOG _POINT shaders[ii].getFilename() _END_
   }
   lEntry_LOG _END
   is_linked = true;
   return theProgram;
}

/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */

/*
 * Addsn attributes to the program when its linked.
 *
 * USE: <number of atrributes> , attributes: [int index, char* name]
 */
void eLinker::setAttributes( GLuint n, ... ) {
   attributes.clear();
   va_list list;
   va_start( list, n );
   for ( unsigned int i = 0; i < n; i++ ) {
      atributeObject attribute;
      attribute.index = va_arg( list, int );
      attribute.name  = va_arg( list, char* );
      attributes.push_back( attribute );
   }
   va_end( list );
}

}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
