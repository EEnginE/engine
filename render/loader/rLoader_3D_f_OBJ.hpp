/*!
 * \file rLoader_3D_OBJ.hpp
 * \brief \b Classes: \a rLoader_3D_OBJ
 */

#ifndef R_LOADER_3D_F_OBJ_H
#define R_LOADER_3D_F_OBJ_H

#include <string>
#include <vector>

#include <GL/glew.h>

namespace e_engine {

namespace functions {struct functionCalls;}

class rLoader_3D_f_OBJ {
   private:
      bool        vIsDataLoaded_B;

      std::string vFilePath_str;
      
      void addVertex( GLfloat _vert ) {vVertexData.push_back( _vert );}
      void addIndex( GLuint _index )  {vIndexData.push_back( _index );}

   protected:

      std::vector<GLfloat> vVertexData;
      std::vector<GLuint>  vIndexData;
            
   public:
      rLoader_3D_f_OBJ();
      rLoader_3D_f_OBJ( std::string _file );
      virtual ~rLoader_3D_f_OBJ() {}

      void setFile( std::string _file );
      int  load();
      void unLoad();
      
      bool        getIsLoaded() const;
      std::string getFilePath() const;
            
      friend struct functions::functionCalls;
};

}

#endif // R_LOADER_3D_F_OBJ_H

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

