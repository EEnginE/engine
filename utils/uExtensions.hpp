#ifndef U_EXTENSIONS_HPP
#define U_EXTENSIONS_HPP

#include <string>
#include <vector>

namespace e_engine {

enum EXTENSIONS {
   ID_ARB_program_interface_query = 0,
   __EXTENSIONS_END__
};

enum OPENGL_VERSIONS {
   OGL_VERSION_NONE = -1,
   OGL_VERSION_1_2  = 0,
   OGL_VERSION_1_3,
   OGL_VERSION_1_4,
   OGL_VERSION_1_5,

   OGL_VERSION_2_0,
   OGL_VERSION_2_1,

   OGL_VERSION_3_0,
   OGL_VERSION_3_1,
   OGL_VERSION_3_2,
   OGL_VERSION_3_3,

   OGL_VERSION_4_0,
   OGL_VERSION_4_1,
   OGL_VERSION_4_2,
   OGL_VERSION_4_3,
   OGL_VERSION_4_4,
};



struct uExtensionData {
   EXTENSIONS  id;
   std::string extStr;
   bool        supported;
};

class uExtensions {
   private:
      uExtensionData *vOpenGLExtList;
      OPENGL_VERSIONS vVersion;

   public:
      uExtensions();
      virtual ~uExtensions();

      OPENGL_VERSIONS querryAll();
      bool isSupported( std::string _ext );
      bool isSupported( EXTENSIONS _id ) {return vOpenGLExtList[_id].supported;}

      OPENGL_VERSIONS getOpenGLVersion() {return vVersion;}
};

}

#endif // UEXTENSIONS_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 


