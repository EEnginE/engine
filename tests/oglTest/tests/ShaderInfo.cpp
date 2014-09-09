/*!
 * \file ShaderInfo.cpp
 * \brief \b Classes: \a ShaderInfo
 * 
 * Class for testing:
 * Tests if it is possible to query shader information
 *
 */

#include "ShaderInfo.hpp"

const string ShaderInfo::desc = "Tests if it is possible to query shader information with the new OGL style";

void ShaderInfo::runTest( uJSON_data &_data, string _dataRoot ) {
   if( ! GlobConf.extensions.isSupported( ID_ARB_program_interface_query ) ) {
      _data( "oglTest", "shader", "queryType", S_NUM( 1 ) );
   }
   
   GlobConf.ogl.shaderInfoQueryType = 2;

   rShader testShader( _dataRoot + "testShader" );
   
   // Compile
   if ( testShader.compile() < 0 ) {
      // Compile error
      _data( "oglTest", "shader", "useShaders", S_BOOL( false ) );
      _data( "oglTest", "shader", "queryType",  S_NUM( 0 ) );
      eLOG "Failed to compile test shader" END
      return;
   }
   
   internal::programInfo info = testShader.getShaderInfo();
   
   _data( "oglTest", "shader", "useShaders", S_BOOL( true ) );
   
   if( ! ( info.vInputInfo.size() == 1 && info.vOutputInfo.size() == 1 && info.vUniformInfo.size() == 1 ) ) {
      _data( "oglTest", "shader", "queryType",  S_NUM( 1 ) );
      wLOG "New shader query failed" END
      return;
   }
   
   if( ! ( info.vInputInfo[0].name == "Position" && info.vInputInfo[0].type == GL_FLOAT_VEC3 && info.vInputInfo[0].location == 4 ) ) {
      _data( "oglTest", "shader", "queryType",  S_NUM( 1 ) );
      wLOG "New shader query failed" END
      return;
   }
   
   if( ! ( info.vOutputInfo[0].name == "FragColor" && info.vOutputInfo[0].type == GL_FLOAT_VEC4 && info.vOutputInfo[0].location == 2 ) ) {
      _data( "oglTest", "shader", "queryType",  S_NUM( 1 ) );
      wLOG "New shader query failed" END
      return;
   }
   
   if( ! ( info.vUniformInfo[0].name == "gWorld" && info.vUniformInfo[0].type == GL_FLOAT_MAT4 ) ) {
      _data( "oglTest", "shader", "queryType",  S_NUM( 1 ) );
      wLOG "New shader query failed" END
      return;
   }
   
   _data( "oglTest", "shader", "queryType",  S_NUM( 2 ) );
   iLOG "New shader query succeeded" END

}

/*
 * Begin recommended Bindings
 * 
 * Syntax: '//#!BIND ' <location in json file> , G_<TYPE>( <GlobConf value>, <default> )
 */
//#!BIND "oglTest", "shader", "queryType",  G_NUM( GlobConf.ogl.shaderInfoQueryType, 0 )
//#!BIND "oglTest", "shader", "useShaders", G_BOOL( GlobConf.ogl.useShaders, true )

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

