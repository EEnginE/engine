#include <GL/glew.h>
#include "rShader.hpp"
#include "uLog.hpp"
#include "defines.hpp"
#include "rShader_structs.hpp"

namespace e_engine {

#if E_DEBUG_LOGGING
std::string getTypeString( int _type ) {
   switch( _type ) {
      case GL_FLOAT:                                     return "float";
      case GL_FLOAT_VEC2:                                return "vec2";
      case GL_FLOAT_VEC3:                                return "vec3";
      case GL_FLOAT_VEC4:                                return "vec4";
      case GL_DOUBLE:                                    return "double";
      case GL_DOUBLE_VEC2:                               return "dvec2";
      case GL_DOUBLE_VEC3:                               return "dvec3";
      case GL_DOUBLE_VEC4:                               return "dvec4";
      case GL_INT:                                       return "int";
      case GL_INT_VEC2:                                  return "ivec2";
      case GL_INT_VEC3:                                  return "ivec3";
      case GL_INT_VEC4:                                  return "ivec4";
      case GL_UNSIGNED_INT:                              return "unsigned int";
      case GL_UNSIGNED_INT_VEC2:                         return "uvec2";
      case GL_UNSIGNED_INT_VEC3:                         return "uvec3";
      case GL_UNSIGNED_INT_VEC4:                         return "uvec4";
      case GL_BOOL:                                      return "bool";
      case GL_BOOL_VEC2:                                 return "bvec2";
      case GL_BOOL_VEC3:                                 return "bvec3";
      case GL_BOOL_VEC4:                                 return "bvec4";
      case GL_FLOAT_MAT2:                                return "mat2";
      case GL_FLOAT_MAT3:                                return "mat3";
      case GL_FLOAT_MAT4:                                return "mat4";
      case GL_FLOAT_MAT2x3:                              return "mat2x3";
      case GL_FLOAT_MAT2x4:                              return "mat2x4";
      case GL_FLOAT_MAT3x2:                              return "mat3x2";
      case GL_FLOAT_MAT3x4:                              return "mat3x4";
      case GL_FLOAT_MAT4x2:                              return "mat4x2";
      case GL_FLOAT_MAT4x3:                              return "mat4x3";
      case GL_DOUBLE_MAT2:                               return "dmat2";
      case GL_DOUBLE_MAT3:                               return "dmat3";
      case GL_DOUBLE_MAT4:                               return "dmat4";
      case GL_DOUBLE_MAT2x3:                             return "dmat2x3";
      case GL_DOUBLE_MAT2x4:                             return "dmat2x4";
      case GL_DOUBLE_MAT3x2:                             return "dmat3x2";
      case GL_DOUBLE_MAT3x4:                             return "dmat3x4";
      case GL_DOUBLE_MAT4x2:                             return "dmat4x2";
      case GL_DOUBLE_MAT4x3:                             return "dmat4x3";
      case GL_SAMPLER_1D:                                return "sampler1D";
      case GL_SAMPLER_2D:                                return "sampler2D";
      case GL_SAMPLER_3D:                                return "sampler3D";
      case GL_SAMPLER_CUBE:                              return "samplerCube";
      case GL_SAMPLER_1D_SHADOW:                         return "sampler1DShadow";
      case GL_SAMPLER_2D_SHADOW:                         return "sampler2DShadow";
      case GL_SAMPLER_1D_ARRAY:                          return "sampler1DArray";
      case GL_SAMPLER_2D_ARRAY:                          return "sampler2DArray";
      case GL_SAMPLER_1D_ARRAY_SHADOW:                   return "sampler1DArrayShadow";
      case GL_SAMPLER_2D_ARRAY_SHADOW:                   return "sampler2DArrayShadow";
      case GL_SAMPLER_2D_MULTISAMPLE:                    return "sampler2DMS";
      case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:              return "sampler2DMSArray";
      case GL_SAMPLER_CUBE_SHADOW:                       return "samplerCubeShadow";
      case GL_SAMPLER_BUFFER:                            return "samplerBuffer";
      case GL_SAMPLER_2D_RECT:                           return "sampler2DRect";
      case GL_SAMPLER_2D_RECT_SHADOW:                    return "sampler2DRectShadow";
      case GL_INT_SAMPLER_1D:                            return "isampler1D";
      case GL_INT_SAMPLER_2D:                            return "isampler2D";
      case GL_INT_SAMPLER_3D:                            return "isampler3D";
      case GL_INT_SAMPLER_CUBE:                          return "isamplerCube";
      case GL_INT_SAMPLER_1D_ARRAY:                      return "isampler1DArray";
      case GL_INT_SAMPLER_2D_ARRAY:                      return "isampler2DArray";
      case GL_INT_SAMPLER_2D_MULTISAMPLE:                return "isampler2DMS";
      case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:          return "isampler2DMSArray";
      case GL_INT_SAMPLER_BUFFER:                        return "isamplerBuffer";
      case GL_INT_SAMPLER_2D_RECT:                       return "isampler2DRect";
      case GL_UNSIGNED_INT_SAMPLER_1D:                   return "usampler1D";
      case GL_UNSIGNED_INT_SAMPLER_2D:                   return "usampler2D";
      case GL_UNSIGNED_INT_SAMPLER_3D:                   return "usampler3D";
      case GL_UNSIGNED_INT_SAMPLER_CUBE:                 return "usamplerCube";
      case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:             return "usampler2DArray";
      case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:             return "usampler2DArray";
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:       return "usampler2DMS";
      case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY: return "usampler2DMSArray";
      case GL_UNSIGNED_INT_SAMPLER_BUFFER:               return "usamplerBuffer";
      case GL_UNSIGNED_INT_SAMPLER_2D_RECT:              return "usampler2DRect";
   }

   return "UNKOWN";
}
#endif

std::string rShader::processData( GLenum _type, GLuint _index, GLsizei _arraySize, GLenum *_in, GLint *_out ) {
   // Get almost all information we can get
   glGetProgramResourceiv(
         vShaderProgram_OGL,        // The program ID
         _type,                     // The program Interface​
         _index,                    // The current index
         _arraySize, _in,           // The inpt array and it's size
         _arraySize, nullptr, _out  // The output array and it's size
   );

   std::string lName_str;
   lName_str.resize( _out[0] );
   glGetProgramResourceName(
         vShaderProgram_OGL,        // The program ID
         _type,                     // The program Interface​
         _index,                    // The current index
         lName_str.size(),          // Our available memory
         nullptr,                   // We dont care about the length
         &lName_str[0]              // The adress of the array in RAM
   );
   lName_str.resize( lName_str.size() - 1 ); // Remove the annoying '\0' at the end
   return lName_str;
}


void rShader::getProgramInfo() {
   if( !vIsShaderLinked_B )
      return;

   if( ! GlobConf.extensions.isSupported( ID_ARB_program_interface_query ) ) {
      wLOG "Extension ARB_program_interface_query is not supported! Can not querry information about the shader" END
      return;
   }

   GLenum lInputValues[]   = { GL_NAME_LENGTH, GL_TYPE, GL_ARRAY_SIZE, GL_LOCATION,                    GL_IS_PER_PATCH, GL_LOCATION_COMPONENT };
   GLenum lOutputValues[]  = { GL_NAME_LENGTH, GL_TYPE, GL_ARRAY_SIZE, GL_LOCATION, GL_LOCATION_INDEX, GL_IS_PER_PATCH, GL_LOCATION_COMPONENT };
   GLenum lUniformValues[] = {
      GL_NAME_LENGTH,
      GL_TYPE,
      GL_ARRAY_SIZE,
      GL_OFFSET,
      GL_BLOCK_INDEX,
      GL_ARRAY_STRIDE,
      GL_MATRIX_STRIDE,
      GL_IS_ROW_MAJOR,
      GL_ATOMIC_COUNTER_BUFFER_INDEX,
      GL_LOCATION
   };
   GLenum lUniformBlockValues[] = {
      GL_NAME_LENGTH,
      GL_BUFFER_BINDING,
      GL_BUFFER_DATA_SIZE,
      GL_NUM_ACTIVE_VARIABLES
   };

   GLint lNumOfInputs;
   GLint lNumOfOutouts;
   GLint lNumOfUniforms;
   GLint lNumOfUniformBlocks;
   glGetProgramInterfaceiv( vShaderProgram_OGL, GL_PROGRAM_INPUT,  GL_ACTIVE_RESOURCES, &lNumOfInputs );
   glGetProgramInterfaceiv( vShaderProgram_OGL, GL_PROGRAM_OUTPUT, GL_ACTIVE_RESOURCES, &lNumOfOutouts );
   glGetProgramInterfaceiv( vShaderProgram_OGL, GL_UNIFORM,        GL_ACTIVE_RESOURCES, &lNumOfUniforms );
   glGetProgramInterfaceiv( vShaderProgram_OGL, GL_UNIFORM_BLOCK,  GL_ACTIVE_RESOURCES, &lNumOfUniformBlocks );

   // Input
   for( GLuint i = 0; i < lNumOfInputs; ++i ) {
      GLint lResults[6];
      std::string lName_str = processData( GL_PROGRAM_INPUT, i, 6, lInputValues, lResults );

#if E_DEBUG_LOGGING
      dLOG  "Shader " ADD vPath_str ADD " Input Interface index " ADD i
      POINT "Name:               " ADD lName_str
      POINT "Type:               " ADD getTypeString( lResults[1] )
      POINT "Array Size:         " ADD lResults[2]
      POINT "Loaction:           " ADD lResults[3]
      POINT "Is Per Patch:       " ADD lResults[4]
      POINT "Location Component: " ADD lResults[5]
      END
#endif

      vProgramInformation.vInputInfo.emplace_back(
            lName_str,
            lResults[1],
            lResults[2],
            lResults[3],
            lResults[4],
            lResults[5]
      );
   }

   // Output
   for( GLuint i = 0; i < lNumOfOutouts; ++i ) {
      GLint lResults[7];
      std::string lName_str = processData( GL_PROGRAM_OUTPUT, i, 7, lOutputValues, lResults );

#if E_DEBUG_LOGGING
      dLOG  "Shader " ADD vPath_str ADD " Output Interface index " ADD i
      POINT "Name:               " ADD lName_str
      POINT "Type:               " ADD getTypeString( lResults[1] )
      POINT "Array Size:         " ADD lResults[2]
      POINT "Loaction:           " ADD lResults[3]
      POINT "Loaction Index:     " ADD lResults[4]
      POINT "Is Per Patch:       " ADD lResults[5]
      POINT "Location Component: " ADD lResults[6]
      END
#endif

      vProgramInformation.vOutputInfo.emplace_back(
            lName_str,
            lResults[1],
            lResults[2],
            lResults[3],
            lResults[4],
            lResults[5],
            lResults[6]
      );
   }

   // Unifrom Blocks
   for( GLuint i = 0; i < lNumOfUniformBlocks; ++i ) {
      GLint lResults[4];
      std::string lName_str = processData( GL_UNIFORM_BLOCK, i, 4, lUniformBlockValues, lResults );

#if E_DEBUG_LOGGING
      dLOG  "Shader " ADD vPath_str ADD " Uniform Block Interface index " ADD i
      POINT "Name:                 " ADD lName_str
      POINT "Buffer Binding:       " ADD lResults[1]
      POINT "Buffer Data Size:     " ADD lResults[2]
      POINT "Num Active Variables: " ADD lResults[3]
      POINT "Index:                " ADD i
      END
#endif

      vProgramInformation.vUniformBlockInfo.emplace_back(
            lName_str,
            lResults[1],
            lResults[2],
            lResults[3],
            i
      );
   }

   // Unifrom
   for( GLuint i = 0; i < lNumOfUniforms; ++i ) {
      GLint lResults[10];
      std::string lName_str = processData( GL_UNIFORM, i, 10, lUniformValues, lResults );


      auto *lWhereToPutInfo = &vProgramInformation.vUniformInfo;

      if( !( lResults[4] < 0 ) ) {
         /*
          * lUniformValues[4] = GL_BLOCK_INDEX
          * Because GL_BLOCK_INDEX is >= 0, this uniform is stored
          * in a uniform block. Now find it and adjust the pointer.
          */

         bool lFound_B = false;

         for( auto & block : vProgramInformation.vUniformBlockInfo ) {
            if( block.index == lResults[4] ) {
               lFound_B = true;
               lWhereToPutInfo = &block.uniforms;
               break;
            }
         }

         if( !lFound_B ) {
            wLOG "Failed to assign uniform '" ADD lName_str ADD "' to block " ADD lResults[4] ADD " (block not found)" END
         }
      }

#if E_DEBUG_LOGGING
      dLOG  "Shader " ADD vPath_str ADD " Uniform Interface index " ADD i
      POINT "Name:                  " ADD lName_str
      POINT "Type:                  " ADD getTypeString( lResults[1] )
      POINT "Array Size:            " ADD lResults[2]
      POINT "Offset:                " ADD lResults[3]
      POINT "Block Index:           " ADD lResults[4]
      POINT "Array Stride:          " ADD lResults[5]
      POINT "Matrix Stride:         " ADD lResults[6]
      POINT "Is Row Major:          " ADD lResults[7]
      POINT "At. Count. Buff. Ind.: " ADD lResults[8]
      POINT "Loaction:              " ADD lResults[9]
      END
#endif

      lWhereToPutInfo->emplace_back(
            lName_str,
            lResults[1],
            lResults[2],
            lResults[3],
            lResults[4],
            lResults[5],
            lResults[6],
            lResults[7],
            lResults[8],
            lResults[9]
      );
   }

   for( auto & block : vProgramInformation.vUniformBlockInfo ) {
      if( block.numActiveVariables != block.uniforms.size() ) {
         wLOG "Uniform Block " ADD block.name ADD " has " ADD block.uniforms.size() ADD " out of " ADD block.numActiveVariables ADD " uniforms" END
      }
   }


   vHasProgramInformation_B = true;
}

bool rShader::getInputLocation( std::string _name, int &_location ) {
   for( auto loc : vProgramInformation.vInputInfo ) {
      if( _name == loc.name ) {
         _location = loc.location;
         return true;
      }
   }
   return false;
}

bool rShader::getOutputLocation( std::string _name, int &_location ) {
   for( auto loc : vProgramInformation.vOutputInfo ) {
      if( _name == loc.name ) {
         _location = loc.location;
         return true;
      }
   }
   return false;
}

bool rShader::getUniformLocation( std::string _name, int &_location ) {
   for( auto loc : vProgramInformation.vUniformInfo ) {
      if( _name == loc.name ) {
         _location = loc.location;
         return true;
      }
   }

   for( auto block : vProgramInformation.vUniformBlockInfo ) {
      for( auto loc : block.uniforms ) {
         if( _name == loc.name ) {
            _location = loc.location;
            return true;
         }
      }
   }
   return false;
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
