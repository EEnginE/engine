/*!
 * \file rLoader_3D_OBJ.hpp
 * \brief \b Classes: \a rLoader_3D_OBJ
 */

#ifndef R_LOADER_3D_F_OBJ_H
#define R_LOADER_3D_F_OBJ_H

#include "rLoaderBase.hpp"
#include <string>
#include <vector>

#include <GL/glew.h>

namespace e_engine {

class rLoader_3D_f_OBJ : public internal::rLoaderBase<GLfloat, GLuint> {
   public:
      rLoader_3D_f_OBJ();
      rLoader_3D_f_OBJ( std::string _file );
      virtual ~rLoader_3D_f_OBJ() {}

      int  load();
};

}

#endif // R_LOADER_3D_F_OBJ_H

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;

