/*!
 * \file rLinker.hpp
 * \brief \b Classes: \a rLinker
 * \sa rShader
 */
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef LINKER_HPP
#define LINKER_HPP

#include <vector>
#include <string>
#include "rShader.hpp"

namespace e_engine {

static const std::string VERT_END( ".vert" );   //!< Standard shader ending for vertex shaders
static const std::string FRAG_END( ".frag" );   //!< Standard shader ending for fragment shaders
static const std::string GEOM_END( ".geom" );   //!< Standard shader ending for geometry shaders


namespace e_engine_internal {
//! A little structure to handle shader information for rLinker
struct atributeObject {
   int         index;
   std::string name;
};
}

/*!
 * \class e_engine::rLinker
 * \brief Class to link a \c GLSL program
 *
 * This class can find \c GLSL files and link them
 * to a \c GLSL program
 *
 */
class rLinker {
   private:
      std::vector<rShader>  shaders;
      std::string           path;
      std::vector<e_engine_internal::atributeObject> attributes;

      std::string           ending[3];
      unsigned int          found_shaders;

      GLhandleARB           theProgram;

      bool                  is_linked;


      unsigned int test_program();

   public:
      static const short int VERT     = 0;
      static const short int FRAG     = 1;
      static const short int GEOM     = 2;

      static const GLushort  PROGRAM_FILES_EXP = 0; // the shader source files are set explicit
      static const GLushort  PROGRAM_FILE_PATH = 1; // search the shader files with the ending

      rLinker();
      rLinker( std::string _path );
      rLinker( std::string _path, GLuint n, ... );
      ~rLinker() {
         deleteProgram();
      }

      /*!
       * \brief Set a new shader path
       * \param _path Basic shader path
       * \returns Nothing
       */
      GLvoid   setShaders( std::string _path ) {
         path = _path;
      }

      /*!
       * \brief Set the standard endings to find shaders automatically.
       * \param _vert vertex-shader ending
       * \param _frag fragment-shader ending
       * \param _geom geometry-shader ending
       * \returns Nothing
       */
      GLvoid   setEndings( std::string _vert, std::string _frag, std::string _geom ) {
         ending[VERT] = _vert;
         ending[FRAG] = _frag;
         ending[GEOM] = _geom;
      }

      /*!
       * \brief Set additional attributes
       * \param n Number of following attribute pairs
       * \returns Nothing
       */
      void   setAttributes( unsigned int n, ... );

      void   clearAttributes() {
         attributes.clear();   //!< Clear the attribute list   \returns Nothing
      }

      int  search_shaders(); //!< Search for shader files   \returns The number of found shader files


      unsigned int link( GLuint &_theProgram );
      unsigned int link();

      void   deleteProgram() {
         glDeleteProgram( theProgram );   //!< Deletes the programm   \returns Nothing
      }


      inline unsigned int getShaderFound() {
         return found_shaders;   //!< Get the number of found shader files  \returns The number of found shader files
      }
      inline bool   getIsLinked()    {
         return is_linked;   //!< Get if the  program is linked        \returns Whether the  program is linked or not
      }
      inline unsigned int getProgram() {                          //!< Get the program                     \returns The program
         if ( is_linked == true ) {
            return theProgram;
         } else {
            return 0;
         }
      }
};

}

#endif // LINKER_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
