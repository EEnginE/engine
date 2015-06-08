/*!
 * \file rShader_info.cpp
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
#include "uLog.hpp"
#include "defines.hpp"
#include "rShader_structs.hpp"

namespace e_engine {

std::string rShader::getTypeString( GLenum _type ) {
   switch ( _type ) {
      case GL_FLOAT: return "float";
      case GL_FLOAT_VEC2: return "vec2";
      case GL_FLOAT_VEC3: return "vec3";
      case GL_FLOAT_VEC4: return "vec4";
      case GL_DOUBLE: return "double";
      case GL_DOUBLE_VEC2: return "dvec2";
      case GL_DOUBLE_VEC3: return "dvec3";
      case GL_DOUBLE_VEC4: return "dvec4";
      case GL_INT: return "int";
      case GL_INT_VEC2: return "ivec2";
      case GL_INT_VEC3: return "ivec3";
      case GL_INT_VEC4: return "ivec4";
      case GL_UNSIGNED_INT: return "unsigned int";
      case GL_UNSIGNED_INT_VEC2: return "uvec2";
      case GL_UNSIGNED_INT_VEC3: return "uvec3";
      case GL_UNSIGNED_INT_VEC4: return "uvec4";
      case GL_BOOL: return "bool";
      case GL_BOOL_VEC2: return "bvec2";
      case GL_BOOL_VEC3: return "bvec3";
      case GL_BOOL_VEC4: return "bvec4";
      case GL_FLOAT_MAT2: return "mat2";
      case GL_FLOAT_MAT3: return "mat3";
      case GL_FLOAT_MAT4: return "mat4";
      case GL_FLOAT_MAT2x3: return "mat2x3";
      case GL_FLOAT_MAT2x4: return "mat2x4";
      case GL_FLOAT_MAT3x2: return "mat3x2";
      case GL_FLOAT_MAT3x4: return "mat3x4";
      case GL_FLOAT_MAT4x2: return "mat4x2";
      case GL_FLOAT_MAT4x3: return "mat4x3";
      case GL_DOUBLE_MAT2: return "dmat2";
      case GL_DOUBLE_MAT3: return "dmat3";
      case GL_DOUBLE_MAT4: return "dmat4";
      case GL_DOUBLE_MAT2x3: return "dmat2x3";
      case GL_DOUBLE_MAT2x4: return "dmat2x4";
      case GL_DOUBLE_MAT3x2: return "dmat3x2";
      case GL_DOUBLE_MAT3x4: return "dmat3x4";
      case GL_DOUBLE_MAT4x2: return "dmat4x2";
      case GL_DOUBLE_MAT4x3: return "dmat4x3";
      case GL_SAMPLER_1D: return "sampler1D";
      case GL_SAMPLER_2D: return "sampler2D";
      case GL_SAMPLER_3D: return "sampler3D";
      case GL_SAMPLER_CUBE: return "samplerCube";
      case GL_SAMPLER_1D_SHADOW: return "sampler1DShadow";
      case GL_SAMPLER_2D_SHADOW: return "sampler2DShadow";
      case GL_SAMPLER_1D_ARRAY: return "sampler1DArray";
      case GL_SAMPLER_2D_ARRAY: return "sampler2DArray";
      case GL_SAMPLER_1D_ARRAY_SHADOW: return "sampler1DArrayShadow";
      case GL_SAMPLER_2D_ARRAY_SHADOW: return "sampler2DArrayShadow";
      case GL_SAMPLER_2D_MULTISAMPLE: return "sampler2DMS";
      case GL_SAMPLER_2D_MULTISAMPLE_ARRAY: return "sampler2DMSArray";
      case GL_SAMPLER_CUBE_SHADOW: return "samplerCubeShadow";
      case GL_SAMPLER_BUFFER: return "samplerBuffer";
      case GL_SAMPLER_2D_RECT: return "sampler2DRect";
      case GL_SAMPLER_2D_RECT_SHADOW: return "sampler2DRectShadow";
      case GL_INT_SAMPLER_1D: return "isampler1D";
      case GL_INT_SAMPLER_2D: return "isampler2D";
      case GL_INT_SAMPLER_3D: return "isampler3D";
      case GL_INT_SAMPLER_CUBE: return "isamplerCube";
      case GL_INT_SAMPLER_1D_ARRAY: return "isampler1DArray";
      case GL_INT_SAMPLER_2D_ARRAY: return "isampler2DArray";
      case GL_INT_SAMPLER_2D_MULTISAMPLE: return "isampler2DMS";
      case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "isampler2DMSArray";
      case GL_INT_SAMPLER_BUFFER: return "isamplerBuffer";
      case GL_INT_SAMPLER_2D_RECT: return "isampler2DRect";
      case GL_UNSIGNED_INT_SAMPLER_1D: return "usampler1D";
      case GL_UNSIGNED_INT_SAMPLER_2D: return "usampler2D";
      case GL_UNSIGNED_INT_SAMPLER_3D: return "usampler3D";
      case GL_UNSIGNED_INT_SAMPLER_CUBE: return "usamplerCube";
      case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY: return "usampler2DArray";
      case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY: return "usampler2DArray";
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE: return "usampler2DMS";
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "usampler2DMSArray";
      case GL_UNSIGNED_INT_SAMPLER_BUFFER: return "usamplerBuffer";
      case GL_UNSIGNED_INT_SAMPLER_2D_RECT: return "usampler2DRect";
   }

   return "UNKNOWN";
}


std::string rShader::processData(
      GLenum _type, GLuint _index, GLsizei _arraySize, GLenum *_in, GLint *_out ) {
   // Get most of the information we can get
   glGetProgramResourceiv( vShaderProgram_OGL, // The program ID
                           _type,              // The program Interface​
                           _index,             // The current index
                           _arraySize,
                           _in, // The input array and it's size
                           _arraySize,
                           nullptr,
                           _out // The output array and it's size
                           );

   GLsizei lSize = _out[0];

   std::string lName_str;
   lName_str.resize( static_cast<size_t>( lSize ) );
   int actualLength = 10;
   glGetProgramResourceName( vShaderProgram_OGL, // The program ID
                             _type,              // The program Interface​
                             _index,             // The current index
                             lSize,              // Our available memory
                             &actualLength,      // The actual length used
                             &lName_str[0]       // The adress of the array in RAM
                             );
   lName_str.resize( static_cast<size_t>( actualLength ) ); // Remove the annoying '\0' at the end
   return lName_str;
}


void rShader::getProgramInfo() {
   if ( !vIsShaderLinked_B )
      return;

   if ( GlobConf.ogl.shaderInfoQueryType == 1 ) {
      getInfoOld();
      return;
   }
   if ( GlobConf.ogl.shaderInfoQueryType == 2 ) {
      getInfoNew();
      return;
   }

   if ( !GlobConf.extensions.isSupported( ID_ARB_program_interface_query ) ) {
      wLOG( "Extension ARB_program_interface_query is not supported! Fallback to old style query" );
      getInfoOld();
      return;
   }

   getInfoNew();
}



//   _   _
//  | \ | |
//  |  \| | _____      __   __ _ _   _  ___ _ __ _ __ _   _
//  | . ` |/ _ \ \ /\ / /  / _` | | | |/ _ \ '__| '__| | | |
//  | |\  |  __/\ V  V /  | (_| | |_| |  __/ |  | |  | |_| |
//  \_| \_/\___| \_/\_/    \__, |\__,_|\___|_|  |_|   \__, |
//                            | |                      __/ |
//                            |_|                     |___/

void rShader::getInfoNew() {
   iLOG( "Querying shader information form ", vPath_str, " [NEW STYLE]" );

   GLenum lInputValues[] = {GL_NAME_LENGTH,
                            GL_TYPE,
                            GL_ARRAY_SIZE,
                            GL_LOCATION,
                            GL_IS_PER_PATCH,
                            GL_LOCATION_COMPONENT};
   GLenum lOutputValues[] = {GL_NAME_LENGTH,
                             GL_TYPE,
                             GL_ARRAY_SIZE,
                             GL_LOCATION,
                             GL_LOCATION_INDEX,
                             GL_IS_PER_PATCH,
                             GL_LOCATION_COMPONENT};
   GLenum lUniformValues[] = {GL_NAME_LENGTH,
                              GL_TYPE,
                              GL_ARRAY_SIZE,
                              GL_OFFSET,
                              GL_BLOCK_INDEX,
                              GL_ARRAY_STRIDE,
                              GL_MATRIX_STRIDE,
                              GL_IS_ROW_MAJOR,
                              GL_ATOMIC_COUNTER_BUFFER_INDEX,
                              GL_LOCATION};
   GLenum lUniformBlockValues[] = {
         GL_NAME_LENGTH, GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE, GL_NUM_ACTIVE_VARIABLES};

   GLint lNumOfInputs = -1;
   GLint lNumOfOutouts = -1;
   GLint lNumOfUniforms = -1;
   GLint lNumOfUniformBlocks = -1;
   glGetProgramInterfaceiv(
         vShaderProgram_OGL, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &lNumOfInputs );
   glGetProgramInterfaceiv(
         vShaderProgram_OGL, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &lNumOfOutouts );
   glGetProgramInterfaceiv( vShaderProgram_OGL, GL_UNIFORM, GL_ACTIVE_RESOURCES, &lNumOfUniforms );
   glGetProgramInterfaceiv(
         vShaderProgram_OGL, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &lNumOfUniformBlocks );

#if D_LOG_SHADER
   dLOG( "Shader ",
         vPath_str,
         ":"
         "\n  - Number of Inputs:         ",
         lNumOfInputs,
         "\n  - Numner of Outputs:        ",
         lNumOfOutouts,
         "\n  - Number of Uniforms:       ",
         lNumOfUniforms,
         "\n  - Number of Uniform Blocks: ",
         lNumOfUniformBlocks );
#endif

   // Input
   for ( GLint i = 0; i < lNumOfInputs; ++i ) {
      GLint lResults[6];
      std::string lName_str =
            processData( GL_PROGRAM_INPUT, static_cast<unsigned>( i ), 6, lInputValues, lResults );

#if D_LOG_SHADER
      dLOG( "Shader ",
            vPath_str,
            " Input Interface index ",
            i,
            "\n  - Name:               ",
            lName_str,
            "\n  - Type:               ",
            getTypeString( static_cast<GLenum>( lResults[1] ) ),
            "\n  - Array Size:         ",
            lResults[2],
            "\n  - Location:           ",
            lResults[3],
            "\n  - Is Per Patch:       ",
            lResults[4],
            "\n  - Location Component: ",
            lResults[5] );
#endif

      vProgramInformation.vInputInfo.emplace_back(
            lName_str, lResults[1], lResults[2], lResults[3], lResults[4], lResults[5] );
   }

   // Output
   for ( GLint i = 0; i < lNumOfOutouts; ++i ) {
      GLint lResults[7];
      std::string lName_str = processData(
            GL_PROGRAM_OUTPUT, static_cast<unsigned>( i ), 7, lOutputValues, lResults );

#if D_LOG_SHADER
      dLOG( "Shader ",
            vPath_str,
            " Output Interface index ",
            i,
            "\n  - Name:               ",
            lName_str,
            "\n  - Type:               ",
            getTypeString( static_cast<GLenum>( lResults[1] ) ),
            "\n  - Array Size:         ",
            lResults[2],
            "\n  - Location:           ",
            lResults[3],
            "\n  - Location Index:     ",
            lResults[4],
            "\n  - Is Per Patch:       ",
            lResults[5],
            "\n  - Location Component: ",
            lResults[6] );
#endif

      vProgramInformation.vOutputInfo.emplace_back( lName_str,
                                                    lResults[1],
                                                    lResults[2],
                                                    lResults[3],
                                                    lResults[4],
                                                    lResults[5],
                                                    lResults[6] );
   }

   // Unifrom Blocks
   for ( GLint i = 0; i < lNumOfUniformBlocks; ++i ) {
      GLint lResults[4];
      std::string lName_str = processData(
            GL_UNIFORM_BLOCK, static_cast<unsigned>( i ), 4, lUniformBlockValues, lResults );

#if D_LOG_SHADER
      dLOG( "Shader ",
            vPath_str,
            " Uniform Block Interface index ",
            i,
            "\n  - Name:                 ",
            lName_str,
            "\n  - Buffer Binding:       ",
            lResults[1],
            "\n  - Buffer Data Size:     ",
            lResults[2],
            "\n  - Num Active Variables: ",
            lResults[3],
            "\n  - Index:                ",
            i );
#endif

      vProgramInformation.vUniformBlockInfo.emplace_back(
            lName_str, lResults[1], lResults[2], lResults[3], i );
   }

   // Unifrom
   for ( GLint i = 0; i < lNumOfUniforms; ++i ) {
      GLint lResults[10];
      std::string lName_str =
            processData( GL_UNIFORM, static_cast<unsigned>( i ), 10, lUniformValues, lResults );


      auto *lWhereToPutInfo = &vProgramInformation.vUniformInfo;

      if ( lResults[4] >= 0 ) {
         /*
          * lUniformValues[4] = GL_BLOCK_INDEX
          * Because GL_BLOCK_INDEX is >= 0, this uniform is stored
          * in a uniform block. Now find it and adjust the pointer.
          */

         bool lFound_B = false;

         for ( auto &block : vProgramInformation.vUniformBlockInfo ) {
            if ( block.index == lResults[4] ) {
               lFound_B = true;
               lWhereToPutInfo = &block.uniforms;
               break;
            }
         }

         if ( !lFound_B ) {
            wLOG( "Failed to assign uniform '",
                  lName_str,
                  "' to block ",
                  lResults[4],
                  " (block not found)" );
         }
      }

#if D_LOG_SHADER
      dLOG( "Shader ",
            vPath_str,
            " Uniform Interface index ",
            i,
            lResults[4] >= 0 ? " [BLOCK]" : "",
            "\n  - Name:                  ",
            lName_str,
            "\n  - Type:                  ",
            getTypeString( static_cast<GLenum>( lResults[1] ) ),
            "\n  - Array Size:            ",
            lResults[2],
            "\n  - Offset:                ",
            lResults[3],
            "\n  - Block Index:           ",
            lResults[4],
            "\n  - Array Stride:          ",
            lResults[5],
            "\n  - Matrix Stride:         ",
            lResults[6],
            "\n  - Is Row Major:          ",
            lResults[7],
            "\n  - At. Count. Buff. Ind.: ",
            lResults[8],
            "\n  - Location:              ",
            lResults[9] );
#endif

      lWhereToPutInfo->emplace_back( lName_str,
                                     lResults[1],
                                     lResults[2],
                                     lResults[3],
                                     lResults[4],
                                     lResults[5],
                                     lResults[6],
                                     lResults[7],
                                     lResults[8],
                                     lResults[9] );
   }

   for ( auto &block : vProgramInformation.vUniformBlockInfo ) {
      if ( static_cast<size_t>( block.numActiveVariables ) != block.uniforms.size() ) {
         wLOG( "Uniform Block ",
               block.name,
               " has ",
               block.uniforms.size(),
               " out of ",
               block.numActiveVariables,
               " uniforms" );
      }
   }


   vHasProgramInformation_B = true;
}


//   _____ _     _
//  |  _  | |   | |
//  | | | | | __| |   __ _ _   _  ___ _ __ _ __ _   _
//  | | | | |/ _` |  / _` | | | |/ _ \ '__| '__| | | |
//  \ \_/ / | (_| | | (_| | |_| |  __/ |  | |  | |_| |
//   \___/|_|\__,_|  \__, |\__,_|\___|_|  |_|   \__, |
//                      | |                      __/ |
//                      |_|                     |___/

void rShader::getInfoOld() {
   // Attributes
   iLOG( "Querying shader information form ", vPath_str, " [OLD STYLE]" );
   wLOG( "Using this function may cause render problems" );

   int lNumAttributes, lAttribMaxLength;
   glGetProgramiv( vShaderProgram_OGL, GL_ACTIVE_ATTRIBUTES, &lNumAttributes );
   glGetProgramiv( vShaderProgram_OGL, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &lAttribMaxLength );

   int lNumUniforms, lUniformMaxLength;
   glGetProgramiv( vShaderProgram_OGL, GL_ACTIVE_UNIFORMS, &lNumUniforms );
   glGetProgramiv( vShaderProgram_OGL, GL_ACTIVE_UNIFORM_MAX_LENGTH, &lUniformMaxLength );

#if D_LOG_SHADER
   dLOG( "Shader ",
         vPath_str,
         ":"
         "\n  - Number of Attributes: ",
         lNumAttributes,
         "\n  - Number of Uniforms:   ",
         lNumUniforms );
#endif

   for ( int i = 0; i < lNumAttributes; ++i ) {
      auto lName_CSTR = new char[static_cast<size_t>( lAttribMaxLength )];
      GLint lArraySize;
      GLenum lType;
      glGetActiveAttrib( vShaderProgram_OGL,
                         static_cast<unsigned>( i ),
                         lAttribMaxLength,
                         nullptr,
                         &lArraySize,
                         &lType,
                         lName_CSTR );

      GLint lLocation = glGetAttribLocation( vShaderProgram_OGL, lName_CSTR );

      if ( lLocation < 0 ) {
         eLOG( "Something went wrong with shader '",
               vPath_str,
               "' -- Invalid input location of '",
               lName_CSTR,
               "' (",
               lLocation,
               ")" );
         continue;
      }

      vProgramInformation.vInputInfo.emplace_back( std::string( lName_CSTR ),
                                                   lType,
                                                   lArraySize,
                                                   lLocation,
                                                   0, // Not supported here
                                                   0  // Not supported here
                                                   );

#if D_LOG_SHADER
      dLOG( "Shader ",
            vPath_str,
            " Input Interface index ",
            i,
            "\n  - Name:               ",
            lName_CSTR,
            "\n  - Type:               ",
            getTypeString( lType ),
            "\n  - Array Size:         ",
            lArraySize,
            "\n  - Location:           ",
            lLocation,
            "\n  - Is Per Patch:       ",
            "NOT SUPPORTED",
            "\n  - Location Component: ",
            "NOT SUPPORTED" );
#endif
   }


   // Uniform blocks

   if ( GlobConf.extensions.getOpenGLVersion() >= OGL_VERSION_3_1 ) {
      // Uniform blocks only with OGL 3.1+

      GLint lNumOfUniformBlocks;
      glGetProgramiv( vShaderProgram_OGL, GL_ACTIVE_UNIFORM_BLOCKS, &lNumOfUniformBlocks );

      // Unifrom Blocks
      for ( GLint i = 0; i < lNumOfUniformBlocks; ++i ) {
         GLint lBinding, lDataSize, lNameLength, lNumUniforms2;
         glGetActiveUniformBlockiv( vShaderProgram_OGL,
                                    static_cast<unsigned>( i ),
                                    GL_UNIFORM_BLOCK_BINDING,
                                    &lBinding );
         glGetActiveUniformBlockiv( vShaderProgram_OGL,
                                    static_cast<unsigned>( i ),
                                    GL_UNIFORM_BLOCK_DATA_SIZE,
                                    &lDataSize );
         glGetActiveUniformBlockiv( vShaderProgram_OGL,
                                    static_cast<unsigned>( i ),
                                    GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
                                    &lNumUniforms2 );
         glGetActiveUniformBlockiv( vShaderProgram_OGL,
                                    static_cast<unsigned>( i ),
                                    GL_UNIFORM_BLOCK_NAME_LENGTH,
                                    &lNameLength );

         auto lName_CSTR = new char[static_cast<size_t>( lNameLength )];

         glGetActiveUniformBlockName(
               vShaderProgram_OGL, static_cast<unsigned>( i ), lNameLength, nullptr, lName_CSTR );

#if D_LOG_SHADER
         dLOG( "Shader ",
               vPath_str,
               " Uniform Block Interface index ",
               i,
               "\n  - Name:                 ",
               lName_CSTR,
               "\n  - Buffer Binding:       ",
               lBinding,
               "\n  - Buffer Data Size:     ",
               lDataSize,
               "\n  - Num Active Variables: ",
               lNumUniforms2,
               "\n  - Index:                ",
               i );
#endif

         vProgramInformation.vUniformBlockInfo.emplace_back(
               std::string( lName_CSTR ), lBinding, lDataSize, lNumUniforms2, i );
      }
   }


   // Uniforms

   for ( int i = 0; i < lNumUniforms; ++i ) {
      auto lName_CSTR = new char[static_cast<size_t>( lUniformMaxLength )];
      GLint lArraySize, lOffset, lBlockIndex, lArrayStride, lMatrixStride, lType, lACBI,
            lIsRowMajor;

      glGetActiveUniformsiv(
            vShaderProgram_OGL, 1, reinterpret_cast<const GLuint *>( &i ), GL_UNIFORM_TYPE, &lType );
      glGetActiveUniformsiv( vShaderProgram_OGL,
                             1,
                             reinterpret_cast<const GLuint *>( &i ),
                             GL_UNIFORM_SIZE,
                             &lArraySize );
      glGetActiveUniformsiv( vShaderProgram_OGL,
                             1,
                             reinterpret_cast<const GLuint *>( &i ),
                             GL_UNIFORM_OFFSET,
                             &lOffset );
      glGetActiveUniformsiv( vShaderProgram_OGL,
                             1,
                             reinterpret_cast<const GLuint *>( &i ),
                             GL_UNIFORM_BLOCK_INDEX,
                             &lBlockIndex );
      glGetActiveUniformsiv( vShaderProgram_OGL,
                             1,
                             reinterpret_cast<const GLuint *>( &i ),
                             GL_UNIFORM_ARRAY_STRIDE,
                             &lArrayStride );
      glGetActiveUniformsiv( vShaderProgram_OGL,
                             1,
                             reinterpret_cast<const GLuint *>( &i ),
                             GL_UNIFORM_MATRIX_STRIDE,
                             &lMatrixStride );
      glGetActiveUniformsiv( vShaderProgram_OGL,
                             1,
                             reinterpret_cast<const GLuint *>( &i ),
                             GL_UNIFORM_IS_ROW_MAJOR,
                             &lIsRowMajor );
      glGetActiveUniformsiv( vShaderProgram_OGL,
                             1,
                             reinterpret_cast<const GLuint *>( &i ),
                             GL_UNIFORM_ATOMIC_COUNTER_BUFFER_INDEX,
                             &lACBI );

      glGetActiveUniformName(
            vShaderProgram_OGL, static_cast<unsigned>( i ), lUniformMaxLength, nullptr, lName_CSTR );

      GLint lLocation = glGetUniformLocation( vShaderProgram_OGL, lName_CSTR );

      auto *lWhereToPutInfo = &vProgramInformation.vUniformInfo;

      if ( lBlockIndex >= 0 && GlobConf.extensions.getOpenGLVersion() >= OGL_VERSION_3_1 ) {
         /*
          * Because GL_BLOCK_INDEX is >= 0, this uniform is stored
          * in a uniform block. Now find it and adjust the pointer.
          */

         bool lFound_B = false;

         for ( auto &block : vProgramInformation.vUniformBlockInfo ) {
            if ( block.index == lBlockIndex ) {
               lFound_B = true;
               lWhereToPutInfo = &block.uniforms;
               break;
            }
         }

         if ( !lFound_B ) {
            wLOG( "Failed to assign uniform '",
                  lName_CSTR,
                  "' to block ",
                  lBlockIndex,
                  " (block not found)" );
         }
      }

#if D_LOG_SHADER
      dLOG( "Shader ",
            vPath_str,
            " Uniform Interface index ",
            i,
            lBlockIndex >= 0 ? " [BLOCK]" : "",
            "\n  - Name:                  ",
            lName_CSTR,
            "\n  - Type:                  ",
            getTypeString( static_cast<GLenum>( lType ) ),
            "\n  - Array Size:            ",
            lArraySize,
            "\n  - Offset:                ",
            lOffset,
            "\n  - Block Index:           ",
            lBlockIndex,
            "\n  - Array Stride:          ",
            lArrayStride,
            "\n  - Matrix Stride:         ",
            lMatrixStride,
            "\n  - Is Row Major:          ",
            lIsRowMajor,
            "\n  - At. Count. Buff. Ind.: ",
            lACBI,
            "\n  - Location:              ",
            lLocation );
#endif

      lWhereToPutInfo->emplace_back( std::string( lName_CSTR ),
                                     lType,
                                     lArraySize,
                                     lOffset,
                                     lBlockIndex,
                                     lArrayStride,
                                     lMatrixStride,
                                     lIsRowMajor,
                                     lACBI,
                                     lLocation );
   }

   for ( auto &block : vProgramInformation.vUniformBlockInfo ) {
      if ( static_cast<size_t>( block.numActiveVariables ) != block.uniforms.size() ) {
         wLOG( "Uniform Block ",
               block.name,
               " has ",
               block.uniforms.size(),
               " out of ",
               block.numActiveVariables,
               " uniforms" );
      }
   }


   vHasProgramInformation_B = true;
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
