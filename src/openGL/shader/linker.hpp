/// \filelinker.hpp
/// \brief \b Classes: \a eLinker
/// \sa e_linker.cpp e_shader.hpp
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
#include "shader.hpp"

namespace e_engine {

static const std::string VERT_END( ".vert" );   //!< Standard shader ending for vertex shaders
static const std::string FRAG_END( ".frag" );   //!< Standard shader ending for fragment shaders
static const std::string GEOM_END( ".geom" );   //!< Standard shader ending for geometry shaders


namespace e_engine_internal {
//! A little structure to handle shader inforamtion for eLinker
struct atributeObject {
   int         index;
   std::string name;
};
}

/*!
 * \class e_engine::eLinker
 * \brief Class to link a \c GLSL programm
 *
 * This class can find \c GLSL files and link them
 * to a \c GLSL programm
 *
 */
class eLinker {
   private:
      std::vector<eShader>  shaders;
      std::string           path;
      std::vector<e_engine_internal::atributeObject> attributes;

      std::string           ending[3];
      GLuint                found_shadrs;

      GLhandleARB           theProgram;

      bool                  is_linked;

      /*!
       * \brief Test if programm is OK
       * \throws eError Can throw when ther is an linking or compiling error
       */
     GLvoid test_program();

      eLinker() {}

   public:
      static const short int VERT     = 0;
      static const short int FRAG     = 1;
      static const short int GEOM     = 2;

      static const GLushort  PROGRAM_FILES_EXP = 0; // the shader source files are set explicit
      static const GLushort  PROGRAM_FILE_PATH = 1; // serch the shader files with the ending

      eLinker( std::string _path );
      eLinker( std::string _path, GLuint n, ... );
      ~eLinker() {deleteProgram();}

      /*!
       * \brief Set a new shader path
       * \param _path Basic shader path
       * \returns Nothing
       */
     GLvoid   setShaders( std::string _path ) {
         path = _path;
         serch_shaders();
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
     GLvoid   setAttributes( GLuint n, ... );

     GLvoid   clearAttributes() {attributes.clear();} //!< Cleare the attribute list   \returns Nothing

      GLint  serch_shaders(); //!< Serch for schader files   \returns The number of found shader files

      /*!
       * \brief Search the shader files, compile them and link the program
       * \throws eError When there is an error
       * \returns Nothing
       */
      GLuint link();

     GLvoid   deleteProgram() { glDeleteProgram( theProgram );} //!< Deletes the programm   \returns Nothing


      inline GLuint getShaderFound() {return found_shadrs;} //!< Get the number of found shader files  \returns The number of found shader files
      inline bool   getIsLinked()    {return is_linked;}    //!< Get if the  programm is linked        \returns Whether the  programm is linked or not
      inline GLuint getProgram() {                          //!< Get the programm                      \returns The programm
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
