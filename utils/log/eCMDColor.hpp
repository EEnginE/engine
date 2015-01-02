/*!
 * \file x11/eCMDColor.hpp
 * \brief \b Classes: \a eCMDColor
 * \sa e_color.cpp
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef E_COLOR_HPP
#define E_COLOR_HPP

#include <string>
#include <GL/glew.h>

#include "defines.hpp"

#if UNIX
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#if 0

#ifndef E_COLOR_DISABLED
#define E_COLOR_DISABLED
#endif

#ifndef E_COLOR_NO_TERMTEST
#define E_COLOR_NO_TERMTEST
#endif

#endif

#if defined E_COLOR_NO_TERMTEST || !UNIX
#define __IOCTL_TERMTEST__
#else
#define __IOCTL_TERMTEST__                                                                         \
   if ( isatty( fileno( stdout ) ) == 0 )                                                          \
      return L"";
#endif

namespace e_engine {

/*!
 * \class e_engine::eCMDColor
 * \brief The main color string changing class for the commandline
 *
 * This class generates a string changing the current color of the commandline.
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
   static const uint16_t OFF = 0;
   static const uint16_t BOLD = 1;
   static const uint16_t UNDERSCORE = 4;
   static const uint16_t BLINK = 5;
   static const uint16_t REVERSE = 7;
   static const uint16_t CONCEALED = 8;

   static const uint16_t BLACK = 30;
   static const uint16_t RED = 31;
   static const uint16_t GREEN = 32;
   static const uint16_t YELLOW = 33;
   static const uint16_t BLUE = 34;
   static const uint16_t MAGENTA = 35;
   static const uint16_t CYAN = 36;
   static const uint16_t WHITE = 37;

   static const std::wstring RESET;

   static inline std::wstring reset();

   static inline std::wstring color( uint16_t _a1 );
   static inline std::wstring color( uint16_t _a1, uint16_t _a2 );
   static inline std::wstring color( uint16_t _a1, uint16_t _a2, uint16_t _a3 );

   static inline std::wstring color( GLchar _fg );
   static inline std::wstring color( GLchar _a, GLchar _fg );
   static inline std::wstring color( GLchar _a, GLchar _fg, GLchar _bg );

   static inline uint16_t charToColorId( GLchar _c );
   static inline uint16_t charToAtributeId( GLchar _c );
};

std::wstring eCMDColor::reset() {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return RESET;
#else  // E_COLOR_DISABLED
   return L"";
#endif // E_COLOR_DISABLED
}


/*!
 * \brief Set a console attribute
 * \param _a The attribute integer
 * \returns An escape sequence for the attribute
 */
std::wstring eCMDColor::color( uint16_t _a1 ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   std::wstring temp = L"\x1b[" + std::to_wstring( _a1 ) + L'm';
   return temp;
#else  // E_COLOR_DISABLED
   return L"";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute and the FG color
 * \param _a  The attribute integer
 * \param _fg The FG color integer
 * \returns An escape sequence for the attribute and the FG color
 */
std::wstring eCMDColor::color( uint16_t _a1, uint16_t _a2 ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   std::wstring temp = L"\x1b[" + std::to_wstring( _a1 ) + L';' + std::to_wstring( _a2 ) + L'm';
   return temp;
#else  // E_COLOR_DISABLED
   return L"";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute, the FG and BG color
 * \param _a  The attribute integer
 * \param _fg The FG color integer
 * \param _bg The BG color integer
 * \returns An escape sequence for the attribute the FG and BG color
 */
std::wstring eCMDColor::color( uint16_t _a1, uint16_t _a2, uint16_t _a3 ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   std::wstring temp = L"\x1b[" + std::to_wstring( _a1 ) + L';' + std::to_wstring( _a2 ) + L';' +
                       std::to_wstring( _a3 ) + L'm';
   return temp;
#else  // E_COLOR_DISABLED
   return L"";
#endif // E_COLOR_DISABLED
}


/*!
 * \brief Set a console attribute
 * \param _a The attribute character
 * \returns An escape sequence for the attribute
 */
std::wstring eCMDColor::color( GLchar _fg ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return color( charToColorId( _fg ) );
#else  // E_COLOR_DISABLED
   return L"";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute and the FG color
 * \param _a  The attribute character
 * \param _fg The FG color character
 * \returns An escape sequence for the attribute and the FG color
 */
std::wstring eCMDColor::color( GLchar _a, GLchar _fg ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return color( charToAtributeId( _a ), charToColorId( _fg ) );
#else  // E_COLOR_DISABLED
   return L"";
#endif // E_COLOR_DISABLED
}

/*!
 * \brief Set a console attribute, the FG and BG color
 * \param _a  The attribute character
 * \param _fg The FG color character
 * \param _bg The BG color character
 * \returns An escape sequence for the attribute the FG and BG color
 */
std::wstring eCMDColor::color( GLchar _a, GLchar _fg, GLchar _bg ) {
#ifndef E_COLOR_DISABLED
   __IOCTL_TERMTEST__

   return color( charToAtributeId( _a ), charToColorId( _fg ), charToColorId( _bg ) + 10 );
#else  // E_COLOR_DISABLED
   return L"";
#endif // E_COLOR_DISABLED
}


uint16_t eCMDColor::charToAtributeId( GLchar _c ) {
   switch ( _c ) {
      case 'O':
         return OFF;
      case 'B':
         return BOLD;
      case 'U':
         return UNDERSCORE;
      case 'L':
         return BLINK;
      case 'R':
         return REVERSE;
      case 'C':
         return CONCEALED;
      default:
         return OFF;
   }
}

uint16_t eCMDColor::charToColorId( GLchar _c ) {
   switch ( _c ) {
      case 'S':
         return BLACK;
      case 'R':
         return RED;
      case 'G':
         return GREEN;
      case 'Y':
         return YELLOW;
      case 'B':
         return BLUE;
      case 'M':
         return MAGENTA;
      case 'C':
         return CYAN;
      case 'W':
         return WHITE;
      default:
         return WHITE;
   }
}
}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
