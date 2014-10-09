#include <GL/glew.h>
#include "uExtensions.hpp"

namespace e_engine {

uExtensions::uExtensions() {
   vVersion = OGL_VERSION_NONE;
   
   
   vOpenGLExtList = new uExtensionData[__EXTENSIONS_END__];

   vOpenGLExtList[ ID_ARB_program_interface_query ] = { ID_ARB_program_interface_query, "GL_ARB_program_interface_query", false };
}

uExtensions::~uExtensions() {
   delete[] vOpenGLExtList;
}



OPENGL_VERSIONS uExtensions::querryAll() {
   for ( unsigned int i = 0; i < __EXTENSIONS_END__; ++i ) {
      vOpenGLExtList[i].supported = glewIsSupported( vOpenGLExtList[i].extStr.c_str() );
   }

   int lMajorVersion, lMinorVersion;

   glGetIntegerv( GL_MAJOR_VERSION, &lMajorVersion );
   glGetIntegerv( GL_MINOR_VERSION, &lMinorVersion );

   switch ( lMajorVersion ) {
      case 1:
         switch ( lMinorVersion ) {
            case 2:  return vVersion = OGL_VERSION_1_2;
            case 3:  return vVersion = OGL_VERSION_1_3;
            case 4:  return vVersion = OGL_VERSION_1_4;
            case 5:  return vVersion = OGL_VERSION_1_5;
            default: return vVersion = OGL_VERSION_NONE;
         }
      case 2:
         switch ( lMinorVersion ) {
            case 0:  return vVersion = OGL_VERSION_2_0;
            case 1:  return vVersion = OGL_VERSION_2_1;
            default: return vVersion = OGL_VERSION_NONE;
         }
      case 3:
         switch ( lMinorVersion ) {
            case 0:  return vVersion = OGL_VERSION_3_0;
            case 1:  return vVersion = OGL_VERSION_3_1;
            case 2:  return vVersion = OGL_VERSION_3_2;
            case 3:  return vVersion = OGL_VERSION_3_3;
            default: return vVersion = OGL_VERSION_NONE;
         }
      case 4:
         switch ( lMinorVersion ) {
            case 0:  return vVersion = OGL_VERSION_4_0;
            case 1:  return vVersion = OGL_VERSION_4_1;
            case 2:  return vVersion = OGL_VERSION_4_2;
            case 3:  return vVersion = OGL_VERSION_4_3;
            case 4:  return vVersion = OGL_VERSION_4_4;
            case 5:  return vVersion = OGL_VERSION_4_5;
            default: return vVersion = OGL_VERSION_NONE;
         }
      default: return vVersion = OGL_VERSION_NONE;
   }
}

bool uExtensions::isSupported( std::string _ext ) {
   return glewIsSupported( _ext.c_str() );
}



}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
