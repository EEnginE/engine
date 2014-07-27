/*!
 * \file rRenderNormal_3_3.cpp
 * \brief \b Classes: \a rRenderNormal_3_3
 */

#include <vector>
#include "rRenderNormal_3_3.hpp"
#include "uLog.hpp"

namespace e_engine {

rRenderNormal_3_3::rRenderNormal_3_3() {
   vVertexBufferObj_OGL = 0;
   vIndexBufferObj_OGL  = 0;
   vShader_OGL          = 0;
   vInputLocation_OGL   = 0;
   vDataSize_uI         = 0;
}


void rRenderNormal_3_3::render() {
   glClear( GL_COLOR_BUFFER_BIT );

   glUseProgram( vShader_OGL );
   glEnableVertexAttribArray( vInputLocation_OGL );
   glBindBuffer( GL_ARRAY_BUFFER, vVertexBufferObj_OGL );
   glVertexAttribPointer( vInputLocation_OGL, 3, GL_FLOAT, GL_FALSE, 0, 0 );
   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObj_OGL );

   glDrawElements( GL_TRIANGLES, vDataSize_uI, GL_UNSIGNED_INT, 0 );

   glDisableVertexAttribArray( vInputLocation_OGL );
}

/*!
 * \brief Sets information for the renderer
 *
 * \warning If no data is set a render() can result in undefined behavior
 *
 * \note you can delete / free the data calling this function
 *
 * \param[in] _data An array of data
 *
 * _data arguments:
 * 
 * | Num |  Type  |      Description      |
 * | :-: | :----: | :-------------------: |
 * | 0   | GLuint | Vertex Buffer Object  |
 * | 1   | GLuint | Index Buffer Object   |
 * | 2   | GLuint | Shader ID             |
 * | 3   | GLint  | Shader input location |
 * | 4   | GLuint | Num of indexes        |
 *
 */
bool rRenderNormal_3_3::setOGLInfo( std::vector< void * > &_data ) {
   if( _data.size() != 5 ) {
      eLOG "This renderer needs exactly 5 data types (1:VBO:GLuint; 1:IBO:GLuint; 1:SHADER:GLuint; 1:INPUT:GLint; 1:SIZE:GLuint)" END
      return false;
   }

   vVertexBufferObj_OGL = *( ( GLuint * )_data[0] );
   vIndexBufferObj_OGL  = *( ( GLuint * )_data[1] );
   vShader_OGL          = *( ( GLuint * )_data[2] );
   vInputLocation_OGL   = *( ( GLint * )_data[3] );
   vDataSize_uI         = *( ( GLuint * )_data[4] );

   return true;
}


}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
