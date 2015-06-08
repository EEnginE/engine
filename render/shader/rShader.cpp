/*!
 * \file rShader.cpp
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


#include <GL/glew.h>
#include "rShader.hpp"
#include <boost/filesystem.hpp>
#include "uLog.hpp"
#include "defines.hpp"
#include "eCMDColor.hpp"
#include <regex>
#include <stdio.h>


namespace e_engine {

std::string getShaderTypeString( GLenum _type );

rShader::rShader() {
   vShaderProgram_OGL = 0;
   vIsShaderLinked_B = false;

   vShaderEndings[VERT] = ".vert";
   vShaderEndings[FRAG] = ".frag";
   vShaderEndings[GEOM] = ".geom";

   // Inputs:

   vInfo[VERTEX_INPUT].uName = "iVertex";
   vInfo[VERTEX_INPUT].type = GL_FLOAT_VEC3;

   vInfo[NORMALS_INPUT].uName = "iNormals";
   vInfo[NORMALS_INPUT].type = GL_FLOAT_VEC3;

   // Uniforms:

   vInfo[MODEL_MATRIX].uName = "uModel";
   vInfo[MODEL_MATRIX].type = GL_FLOAT_MAT4;

   vInfo[VIEW_MATRIX].uName = "uView";
   vInfo[VIEW_MATRIX].type = GL_FLOAT_MAT4;

   vInfo[MODEL_VIEW_MATRIX].uName = "uModelView";
   vInfo[MODEL_VIEW_MATRIX].type = GL_FLOAT_MAT4;

   vInfo[PROJECTOIN_MATRIX].uName = "uProjection";
   vInfo[PROJECTOIN_MATRIX].type = GL_FLOAT_MAT4;

   vInfo[M_V_P_MATRIX].uName = "uMVP";
   vInfo[M_V_P_MATRIX].type = GL_FLOAT_MAT4;

   vInfo[NORMAL_MATRIX].uName = "uNormal";
   vInfo[NORMAL_MATRIX].type = GL_FLOAT_MAT3;

   vInfo[LIGHT_TYPE].uName = "type";
   vInfo[LIGHT_TYPE].sName = "uLights";
   vInfo[LIGHT_TYPE].type = GL_INT;

   vInfo[AMBIENT_COLOR].uName = "ambient";
   vInfo[AMBIENT_COLOR].sName = "uLights";
   vInfo[AMBIENT_COLOR].type = GL_FLOAT_VEC3;

   vInfo[LIGHT_COLOR].uName = "color";
   vInfo[LIGHT_COLOR].sName = "uLights";
   vInfo[LIGHT_COLOR].type = GL_FLOAT_VEC3;

   vInfo[LIGHT_POSITION].uName = "position";
   vInfo[LIGHT_POSITION].sName = "uLights";
   vInfo[LIGHT_POSITION].type = GL_FLOAT_VEC3;

   vInfo[LIGHT_ATTENUATION].uName = "attenuation";
   vInfo[LIGHT_ATTENUATION].sName = "uLights";
   vInfo[LIGHT_ATTENUATION].type = GL_FLOAT_VEC3;

   vInfo[NUM_LIGHTS].uName = "uNumLights";
   vInfo[NUM_LIGHTS].type = GL_INT;
}

rShader::rShader( rShader &&_s )
    : vShaders( std::move( _s.vShaders ) ),
      vPath_str( std::move( _s.vPath_str ) ),
      vShaderProgram_OGL( std::move( _s.vShaderProgram_OGL ) ),
      vIsShaderLinked_B( std::move( _s.vIsShaderLinked_B ) ),
      vProgramInformation( std::move( _s.vProgramInformation ) ),
      vHasProgramInformation_B( std::move( _s.vHasProgramInformation_B ) ) {

   for ( unsigned int i = 0; i < 3; ++i )
      vShaderEndings[i] = std::move( _s.vShaderEndings[i] );

   for ( unsigned int i = 0; i < __END_INF__; ++i )
      vInfo[i] = std::move( _s.vInfo[i] );
}


//   _     _       _      _   _                _               _
//  | |   (_)     | |    | | | |              | |             | |
//  | |    _ _ __ | | __ | |_| |__   ___   ___| |__   __ _  __| | ___ _ __
//  | |   | | '_ \| |/ / | __| '_ \ / _ \ / __| '_ \ / _` |/ _` |/ _ \ '__|
//  | |___| | | | |   <  | |_| | | |  __/ \__ \ | | | (_| | (_| |  __/ |
//  \_____/_|_| |_|_|\_\  \__|_| |_|\___| |___/_| |_|\__,_|\__,_|\___|_|
//

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
      if ( boost::filesystem::exists( vert ) ) // Look for the vertex-shader
         if ( boost::filesystem::is_regular_file( vert ) )
            vShaders.emplace_back( temp[VERT], GL_VERTEX_SHADER );

      if ( boost::filesystem::exists( frag ) ) // Look for the fragment-shader
         if ( boost::filesystem::is_regular_file( vert ) )
            vShaders.emplace_back( temp[FRAG], GL_FRAGMENT_SHADER );

      if ( boost::filesystem::exists( geom ) ) // Look for the geometry-shader
         if ( boost::filesystem::is_regular_file( geom ) )
            vShaders.emplace_back( temp[GEOM], GL_GEOMETRY_SHADER );


   } catch ( const boost::filesystem::filesystem_error &ex ) { eLOG( ex.what() ); } catch ( ... ) {
      eLOG( "Caught unknown exception" );
   }

   if ( vShaders.size() == 0 ) {
      wLOG( "No shaders found! WARNING!" );
   }

   return static_cast<int>( vShaders.size() );
}


bool rShader::addShader( std::string _filename, GLenum _shaderType ) {
   boost::filesystem::path lShaderPath( _filename );

   try {
      if ( boost::filesystem::exists( lShaderPath ) ) { // Look for the vertex-shader
         if ( boost::filesystem::is_regular_file( lShaderPath ) ) {

            switch ( _shaderType ) {
               case GL_VERTEX_SHADER:
                  vShaders.emplace_back( _filename, GL_VERTEX_SHADER );
                  return true;
               case GL_FRAGMENT_SHADER:
                  vShaders.emplace_back( _filename, GL_FRAGMENT_SHADER );
                  return true;
               case GL_GEOMETRY_SHADER:
                  vShaders.emplace_back( _filename, GL_GEOMETRY_SHADER );
                  return true;
               default:
                  eLOG( "Unknown shader type ", _shaderType, " of ", _filename );
                  return false;
            }
         }
      }
   } catch ( const boost::filesystem::filesystem_error &ex ) { eLOG( ex.what() ); } catch ( ... ) {
      eLOG( "Caught unknown exception" );
   }

   eLOG( "Failed to access shader file ", _filename );
   return false;
}


/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ***
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 */
/*!
 * \brief Test if program is OK
 * \returns 1 When successful
 * \returns 2 When a shader linking error occured
 */
unsigned int rShader::testProgram() {
   int status;
   glGetProgramiv( vShaderProgram_OGL, GL_LINK_STATUS, &status );
   if ( status == GL_FALSE ) {
      GLint lLinkLogLength;
      glGetProgramiv( vShaderProgram_OGL, GL_INFO_LOG_LENGTH, &lLinkLogLength );
      auto log = new GLchar[static_cast<size_t>( lLinkLogLength )];
      glGetProgramInfoLog( vShaderProgram_OGL, lLinkLogLength, nullptr, log );

      eLOG( "Linking failure:\n"
            "###################################################################################"
            "\n\n",
            std::string( log ),
            "\n\n##################################################################################"
            "#"

            );
      glDeleteProgram( vShaderProgram_OGL );

      vIsShaderLinked_B = false;

      delete[] log;

      // Return a shader linking error
      return 2;
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
   auto lRet_uI = compile();
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
   if ( vPath_str.empty() && vShaders.empty() ) {
      eLOG( "No shaders set for compilation" );
      return -6;
   }

   if ( search_shaders() == 0 && vShaders.empty() ) {
      eLOG( "Unable to find any shader file ( Path: ", vPath_str, " )" );
      // Return a file not found error
      return -3;
   }

   for ( auto &s : vShaders ) {
      // Compile shaders
      // Check for errors
      if ( s.compileShader() != 1 )
         return -4;
   }


   // Createng the program
   vShaderProgram_OGL = glCreateProgram();

   // Adding shaders
   for ( auto &s : vShaders ) {
      glAttachShader( vShaderProgram_OGL, s.vShader_OGL );
   }

   // Linking
   glLinkProgram( vShaderProgram_OGL );

   // Delete old shaders. Not needed anymore
   for ( auto &s : vShaders ) {
      glDeleteShader( s.vShader_OGL );
   }

   // Linking successful? Returns a shader linking error if unsuccessful
   if ( testProgram() != 1 )
      return -5;


   // Output
   iLOG( "Program with the ", vShaders.size(), " shader(s) successfully linked" );
   for ( auto &s : vShaders ) {
      LOG( _hI, eCMDColor::color( 'O', 'C' ), "  - ", s.vFilename_str );
   }
   vIsShaderLinked_B = true;

   int lTempShaderCounter_I = static_cast<int>( vShaders.size() );
   vShaders.clear();
   getProgramInfo();

   return lTempShaderCounter_I;
}


/*!
 * \brief Deletes the programm
 */
void rShader::deleteProgram() {
   if ( !vIsShaderLinked_B )
      return; // Don't delete twice
   glDeleteProgram( vShaderProgram_OGL );
   vIsShaderLinked_B = false;
}

//   _____                       _ _        _   _                _               _
//  /  __ \                     (_) |      | | | |              | |             | |
//  | /  \/ ___  _ __ ___  _ __  _| | ___  | |_| |__   ___   ___| |__   __ _  __| | ___ _ __
//  | |    / _ \| '_ ` _ \| '_ \| | |/ _ \ | __| '_ \ / _ \ / __| '_ \ / _` |/ _` |/ _ \ '__|
//  | \__/\ (_) | | | | | | |_) | | |  __/ | |_| | | |  __/ \__ \ | | | (_| | (_| |  __/ |
//   \____/\___/|_| |_| |_| .__/|_|_|\___|  \__|_| |_|\___| |___/_| |_|\__,_|\__,_|\___|_|
//                        | |
//                        |_|

std::string getShaderTypeString( GLenum _type ) {
   switch ( _type ) {
      case GL_VERTEX_SHADER_ARB: return "vertex";
      case GL_FRAGMENT_SHADER_ARB: return "fragment";
      case GL_GEOMETRY_SHADER_ARB: return "geometry";
      default: return "unknown";
   }
}

bool rShader::singleShader::readShader() {
   FILE *lFile = fopen( vFilename_str.c_str(), "r" );
   if ( lFile == nullptr ) {
      eLOG( "Unable to open ", vFilename_str );
      return false;
   }

   int c;
   vData_str.clear();

   while ( ( c = fgetc( lFile ) ) != EOF )
      vData_str.append( 1, static_cast<char>( c ) );

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
   if ( test == GL_FALSE ) {
      GLint logLength = 0;
      glGetShaderiv( vShader_OGL, GL_INFO_LOG_LENGTH, &logLength );
      auto log = new GLchar[static_cast<size_t>( logLength )];
      glGetShaderInfoLog( vShader_OGL, logLength, nullptr, log );

      eLOG( "Compile failure in the ",
            getShaderTypeString( vShaderType ),
            " shader ",
            vFilename_str,
            ":\n",
            "###################################################################################"
            "\n\n",
            std::string( log ),
            "\n\n##################################################################################"
            "#" );
      glDeleteShader( vShader_OGL );

      delete[] log;

      // Returns a shader compilation error
      return false;
   }
   return true;
}

/*!
 * This runs read_shader, compiles
 * the shader and returns it
 *
 * \returns 1 When successful
 * \returns 2 When a file-reading error occurs
 * \returns 3 When a compile error occurs
 */
int rShader::singleShader::compileShader() {
   vData_str.clear();
   if ( readShader() == false ) {
      eLOG( "Error while reading source file '", vFilename_str, "'" );
      // Return the file-reading error
      return 2;
   }

   const GLcharARB *temp = vData_str.c_str();
   // Create Shader
   vShader_OGL = glCreateShader( vShaderType );
   // Adding source
   glShaderSource( vShader_OGL, 1, &temp, nullptr );
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
bool rShader::getProgram( unsigned int &_program ) const {
   if ( !vIsShaderLinked_B )
      return false;
   _program = vShaderProgram_OGL;
   return true;
}



/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ***
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 */


void rShader::setUniformTypeString( SHADER_INFORMATION _type, std::string _str ) {
   vInfo[_type].uName = _str;
}

void rShader::setUniformStructString( SHADER_INFORMATION _type, std::string _str ) {
   vInfo[_type].sName = _str;
}

unsigned int rShader::getUniformArraySize( SHADER_INFORMATION _type ) const {
   return static_cast<unsigned>( vInfo[_type].locations.size() );
}


/*!
 * \brief Interpretes the information in the shader info struct
 *
 * This function Interpretes the information in the shader info struct and assigns
 * the location of the input / uniform to a specific type.
 *
 * Defult strings:
 *
 * |    name     |        type       |
 * | :---------: | :---------------: |
 * | iVertex     | VERTEX_INPUT      |
 * | iNormals    | NORMALS_INPUT     |
 * | uModel      | MODEL_MATRIX      |
 * | uView       | VIEW_MATRIX       |
 * | uProjection | PROJECTOIN_MATRIX |
 * | uMVP        | M_V_P_MATRIX      |
 *
 * \returns true if everything went fine and false when at least one value could not be assigned
 */
bool rShader::parseRawInformation() {
   if ( !vIsShaderLinked_B )
      return false;

#if E_DEBUG_LOGGING
   dLOG( "Assigning locations of shader '", vPath_str, "' to a type" );
#endif

   GLint j;
   bool lRet = true;

   std::string lTempName;
   std::string lName;

   for ( auto const &i : vProgramInformation.vInputInfo ) {
      std::string lArrayIndex;
      unsigned int lIndex = 0;

      lName.clear();
      for ( auto it = i.name.begin(); it != i.name.end(); ++it ) {
         if ( *it == '[' ) {
            ++it;
            for ( ; it != i.name.end(); ++it ) {
               if ( *it == ']' ) {
                  ++it;
                  break;
               }
               lArrayIndex += *it;
            }
         }
         lName += *it;
      }

      if ( !lArrayIndex.empty() ) {
         int lTemp = atoi( lArrayIndex.c_str() );
         lIndex = lTemp < 0 ? 0 : static_cast<unsigned>( lTemp );
      }

      for ( j = 0; j < __BEGIN_UNIFORMS__; ++j ) {
         if ( vInfo[j].sName.empty() )
            lTempName = vInfo[j].uName;
         else
            lTempName = vInfo[j].sName + '.' + vInfo[j].uName;

         if ( lName == lTempName && i.type == vInfo[j].type ) {
            if ( lIndex >= vInfo[j].locations.size() )
               vInfo[j].locations.resize( lIndex + 1 );

            vInfo[j].locations[lIndex] = i.location;
            j = -1;
            break;
         }
      }

      if ( j >= 0 ) {
         wLOG( "  - Failed to assign input '",
               i.name,
               "' [",
               i.location,
               "; ",
               getTypeString( static_cast<GLenum>( i.type ) ),
               "]" );
         lRet = false;
      }
   }

   for ( auto const &i : vProgramInformation.vUniformInfo ) {
      std::string lArrayIndex;
      unsigned int lIndex = 0;

      lName.clear();
      for ( auto it = i.name.begin(); it != i.name.end(); ++it ) {
         if ( *it == '[' ) {
            ++it;
            for ( ; it != i.name.end(); ++it ) {
               if ( *it == ']' ) {
                  ++it;
                  break;
               }
               lArrayIndex += *it;
            }
         }
         lName += *it;
      }

      if ( !lArrayIndex.empty() ) {
         int lTemp = atoi( lArrayIndex.c_str() );
         lIndex = lTemp < 0 ? 0 : static_cast<unsigned>( lTemp );
      }

      for ( j = __BEGIN_UNIFORMS__ + 1; j < __END_INF__; ++j ) {
         if ( vInfo[j].sName.empty() )
            lTempName = vInfo[j].uName;
         else
            lTempName = vInfo[j].sName + '.' + vInfo[j].uName;

         if ( lName == lTempName && i.type == vInfo[j].type ) {
            if ( lIndex >= vInfo[j].locations.size() )
               vInfo[j].locations.resize( lIndex + 1 );

            vInfo[j].locations[lIndex] = i.location;
            j = -1;
            break;
         }
      }

      if ( j >= 0 ) {
         wLOG( "  - Failed to assign uniform '",
               i.name,
               "' [",
               i.location,
               "; ",
               getTypeString( static_cast<GLenum>( i.type ) ),
               "]" );
         lRet = false;
      }
   }

   return lRet;
}

GLint rShader::getLocation( SHADER_INFORMATION _type, unsigned int _index ) const {
   if ( _index >= vInfo[_type].locations.size() )
      return -1;

   return vInfo[_type].locations[_index];
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
