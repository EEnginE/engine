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
#include "engine_render_Export.hpp"

namespace e_engine {

class render_EXPORT rLoader_3D_f_OBJ : public internal::rLoaderBase<GLfloat> {
   public:
      rLoader_3D_f_OBJ();
      rLoader_3D_f_OBJ( std::string _file );
      virtual ~rLoader_3D_f_OBJ() {}

      int  load();
};

}

#endif // R_LOADER_3D_F_OBJ_H

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;

