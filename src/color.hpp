/*!
 * \file x11/color.hpp
 * \brief \b Classes: \a eCMDColor
 * \sa e_color.cpp
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

#ifndef E_COLOR_HPP
#define E_COLOR_HPP

#include <string>
#include <GL/glew.h>
#include <boost/lexical_cast.hpp>

#include "defines.hpp"

#if UNIX
#include <sys/ioctl.h>
#include <stdio.h>
#else

#ifndef E_COLOR_DISABLED
#define E_COLOR_DISABLED
#endif

#ifndef E_COLOR_NO_TERMTEST
#define E_COLOR_NO_TERMTEST
#endif

#endif

#if defined E_COLOR_NO_TERMTEST
#define __IOCTL_TERMTEST__
#else
#define __IOCTL_TERMTEST__ if ( isatty( fileno( stdout ) ) == 0 ) return "";
#endif

namespace e_engine {

/*!
 * \class e_engine::eCMDColor
 * \brief The main color string changing class for the commandline
 *
 * This class gnerates a string changing the current color of the commandline.
 *
 * \par Linux
 *
 * On UNIX like systems the terminal output color is controlled
 * by escape sequences. It starts with the hexcode 0x1B followed
 * by an opening square bracket. Next up there is an integer which
 * defines the attribute. After that the next integers are for their
 * foreground and the background colors. The numbers are separated
 * by a ';'. To signal the end a 'm' is used.
 *
 * The possible forms are:
 * \code
 * 0x1B[<Attribute>;<FG Color>;<BG Color>m
 * 0x1B[<Attribute>;<FG Color>m
 * 0x1B[<Attribute>m
 * \endcode
 *
 *
 * \par Linux attribute list
 *
 * A list of the most important attributes with the integer and their
 * associated character:
 *
 * \c Off         - \a 0  - \b 'O'
 * \c Bold        - \a 1  - \b 'B'
 * \c Underscore  - \a 4  - \b 'U'
 * \c Blink       - \a 5  - \b 'L'
 * \c Reverse     - \a 7  - \b 'R'
 * \c Concealed   - \a 8  - \b 'C'
 *
 *
 * \par Linux foreground color list
 *
 * A list of all foreground colors with the integer and their associated
 * character:
 *
 * \c Black       - \a 30 - \b 'S'
 * \c Red         - \a 31 - \b 'R'
 * \c Green       - \a 32 - \b 'G'
 * \c Yellow      - \a 33 - \b 'Y'
 * \c Blue        - \a 34 - \b 'B'
 * \c Magenta     - \a 35 - \b 'M'
 * \c Cyan        - \a 36 - \b 'C'
 * \c White       - \a 37 - \b 'W'
 *
 *
 * \par Linux background color list
 *
 * A list of all background colors with the integer and their associated
 * character:
 *
 * \c Black       - \a 40 - \b 'S'
 * \c Red         - \a 41 - \b 'R'
 * \c Green       - \a 42 - \b 'G'
 * \c Yellow      - \a 43 - \b 'Y'
 * \c Blue        - \a 44 - \b 'B'
 * \c Magenta     - \a 45 - \b 'M'
 * \c Cyan        - \a 46 - \b 'C'
 * \c White       - \a 47 - \b 'W'
 *
 */
struct eCMDColor {
   static const GLuint OFF        = 0;
   static const GLuint BOLD       = 1;
   static const GLuint UNDERSCORE = 4;
   static const GLuint BLINK      = 5;
   static const GLuint REVERSE    = 7;
   static const GLuint CONCEALED  = 8;

   static const GLuint BLACK      = 30;
   static const GLuint RED        = 31;
   static const GLuint GREEN      = 32;
   static const GLuint YELLOW     = 33;
   static const GLuint BLUE       = 34;
   static const GLuint MAGENTA    = 35;
   static const GLuint CYAN       = 36;
   static const GLuint WHITE      = 37;

   static const std::string RESET;
   
   static inline std::string reset();         

   static inline std::string color( GLuint _a1 );
   static inline std::string color( GLuint _a1 , GLuint _a2 );
   static inline std::string color( GLuint _a1 , GLuint _a2 , GLuint _a3 );

   static inline std::string color( GLchar _fg );
   static inline std::string color( GLchar _a, GLchar _fg );
   static inline std::string color( GLchar _a, GLchar _fg , GLchar _bg );

   static inline std::string color( GLuint _a, GLchar _fg );
   static inline std::string color( GLuint _a, GLchar _fg , GLchar _bg );

   static inline GLuint charToColorId( GLchar _c );
   static inline GLuint charToAtributeId( GLchar _c );
};

std::string eCMDColor::reset() {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return RESET;
#else // E_COLOR_DISABLED
   return "";
#endif // E_COLOR_DISABLED
}


/*!
 * \brief Set a console attribute
 * \param _a The attribute integer
 * \returns An escape sequence for the attribute
 */
std::string eCMDColor::color( GLuint _a1 ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   std::string temp = "\x1b[" +
                      boost::lexical_cast<std::string>( _a1 ) + 'm';
   return temp;
#else // E_COLOR_DISABLED
   return "";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute and the FG color
 * \param _a  The attribute integer
 * \param _fg The FG color integer
 * \returns An escape sequence for the attribute and the FG color
 */
std::string eCMDColor::color( GLuint _a1, GLuint _a2 ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   std::string temp = "\x1b[" +
                      boost::lexical_cast<std::string>( _a1 )  + ';' +
                      boost::lexical_cast<std::string>( _a2 ) + 'm';
   return temp;
#else // E_COLOR_DISABLED
   return "";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute, the FG and BG color
 * \param _a  The attribute integer
 * \param _fg The FG color integer
 * \param _bg The BG color integer
 * \returns An escape sequence for the attribute the FG and BG color
 */
std::string eCMDColor::color( GLuint _a1, GLuint _a2, GLuint _a3 ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   std::string temp = "\x1b[" +
                      boost::lexical_cast<std::string>( _a1 )  + ';' +
                      boost::lexical_cast<std::string>( _a2 ) + ';' +
                      boost::lexical_cast<std::string>( _a3 ) + 'm';
   return temp;
#else // E_COLOR_DISABLED
   return "";
#endif // E_COLOR_DISABLED
}


/*!
 * \brief Set a console attribute
 * \param _a The attribute character
 * \returns An escape sequence for the attribute
 */
std::string eCMDColor::color( GLchar _fg ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return color( charToColorId( _fg ) );
#else // E_COLOR_DISABLED
   return "";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute and the FG color
 * \param _a  The attribute character
 * \param _fg The FG color character
 * \returns An escape sequence for the attribute and the FG color
 */
std::string eCMDColor::color( GLchar _a, GLchar _fg ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return color( charToAtributeId( _a ), charToColorId( _fg ) );
#else // E_COLOR_DISABLED
   return "";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute, the FG and BG color
 * \param _a  The attribute character
 * \param _fg The FG color character
 * \param _bg The BG color character
 * \returns An escape sequence for the attribute the FG and BG color
 */
std::string eCMDColor::color( GLchar _a, GLchar _fg, GLchar _bg ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return color( charToAtributeId( _a ), charToColorId( _fg ), charToColorId( _bg ) + 10 );
#else // E_COLOR_DISABLED
   return "";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute, the FG and BG color
 * \param _a  The attribute character
 * \param _fg The FG color character
 * \returns An escape sequence for the attribute the FG and BG color
 */
std::string eCMDColor::color( GLuint _a, GLchar _fg ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return color( _a, charToColorId( _fg ) );
#else // E_COLOR_DISABLED
   return "";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute, the FG and BG color
 * \param _a  The attribute character
 * \param _fg The FG color character
 * \param _bg The BG color character
 * \returns An escape sequence for the attribute the FG and BG color
 */
std::string eCMDColor::color( GLuint _a, GLchar _fg, GLchar _bg ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return color( _a, charToColorId( _fg ), charToColorId( _bg ) + 10 );
#else // E_COLOR_DISABLED
   return "";
#endif // E_COLOR_DISABLED
}



GLuint eCMDColor::charToAtributeId( GLchar _c ) {
   switch ( _c ) {
      case 'O': return OFF;
      case 'B': return BOLD;
      case 'U': return UNDERSCORE;
      case 'L': return BLINK;
      case 'R': return REVERSE;
      case 'C': return CONCEALED;
      default:  return OFF;
   }
}

GLuint eCMDColor::charToColorId( GLchar _c ) {
   switch ( _c ) {
      case 'S': return BLACK;
      case 'R': return RED;
      case 'G': return GREEN;
      case 'Y': return YELLOW;
      case 'B': return BLUE;
      case 'M': return MAGENTA;
      case 'C': return CYAN;
      case 'W': return WHITE;
      default:  return WHITE;
   }
}


}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
