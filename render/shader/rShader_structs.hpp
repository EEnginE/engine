#ifndef R_LINKER_STRUCTS_HPP
#define R_LINKER_STRUCTS_HPP

#include <string>
#include <vector>
#include "engine_render_Export.hpp"


namespace e_engine {

namespace internal {
//! A little structure to handle shader information for rShader
struct atributeObject {
   int         index;
   std::string name;
};

struct __info_Input__ {
   std::string name;
   int         type;              // GL_TYPE
   int         arraySize;         // GL_ARRAY_SIZE
   int         location;          // GL_LOCATION
   int         isPerPatch;        // GL_IS_PER_PATCH
   int         locationComponent; // GL_LOCATION_COMPONENT

   __info_Input__( std::string _name, int _type, int _aSize, int _loc, int _iPP, int _locComp ) :
      name( _name ),
      type( _type ),
      arraySize( _aSize ),
      location( _loc ),
      isPerPatch( _iPP ),
      locationComponent( _locComp )
   {}
};

struct __info_Output__ {
   std::string name;
   int         type;              // GL_TYPE
   int         arraySize;         // GL_ARRAY_SIZE
   int         location;          // GL_LOCATION
   int         locationIndex;     // GL_LOCATION_INDEX
   int         isPerPatch;        // GL_IS_PER_PATCH
   int         locationComponent; // GL_LOCATION_COMPONENT

   __info_Output__( std::string _name, int _type, int _aSize, int _loc, int _locIndex, int _iPP, int _locComp ) :
      name( _name ),
      type( _type ),
      arraySize( _aSize ),
      location( _loc ),
      locationIndex( _locIndex ),
      isPerPatch( _iPP ),
      locationComponent( _locComp )
   {}
};

struct __info_Uniform__ {
   std::string name;
   int         type;                     // GL_TYPE
   int         arraySize;                // GL_ARRAY_SIZE
   int         offset;                   // GL_OFFSET
   int         blockIndex;               // GL_BLOCK_INDEX
   int         arrayStride;              // GL_ARRAY_STRIDE
   int         matrixStride;             // GL_MATRIX_STRIDE
   int         isRowMajor;               // GL_IS_ROW_MAJOR
   int         atomicCounterBufferIndex; // GL_ATOMIC_COUNTER_BUFFER_INDEX​
   int         location;                 // GL_LOCATION

   __info_Uniform__(
         std::string _name,
         int _type,
         int _aSize,
         int _offset,
         int _blockIndex,
         int _arrayStride,
         int _matrixStride,
         int _isRowMajor,
         int _atomicCBI,
         int _loc
   ) :
      name( _name ),
      type( _type ),
      arraySize( _aSize ),
      offset( _offset ),
      blockIndex( _blockIndex ),
      arrayStride( _arrayStride ),
      matrixStride( _matrixStride ),
      isRowMajor( _isRowMajor ),
      atomicCounterBufferIndex( _atomicCBI ),
      location( _loc )
   {}
};




struct __info_Uniform_Block__ {
   std::string name;
   int bufferBinding;      // GL_BUFFER_BINDING
   int bufferDataSize;     // GL_BUFFER_DATA_SIZE
   int numActiveVariables; // GL_NUM_ACTIVE_VARIABLES
   int index;

   std::vector<__info_Uniform__> uniforms;

   __info_Uniform_Block__(
         std::string _name,
         int         _bufferBinding,
         int         _bufferDataSize,
         int         _numActivVars,
         int         _index
   ) :
      name( _name ),
      bufferBinding( _bufferBinding ),
      bufferDataSize( _bufferDataSize ),
      numActiveVariables( _numActivVars ),
      index( _index ),
      uniforms()
   {}
};

struct programInfo {
   std::vector<__info_Input__>         vInputInfo;
   std::vector<__info_Output__>        vOutputInfo;
   std::vector<__info_Uniform__>       vUniformInfo;
   std::vector<__info_Uniform_Block__> vUniformBlockInfo;
};

}

}





#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
