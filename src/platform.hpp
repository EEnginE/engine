/*!
 * \file platform.hpp
 * \brief Handle platform specific stuff
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

#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#if !defined(WIN32) && (defined(_WIN32)) || defined(__WIN32__) || defined(__WIN32) || defined(__CYGWIN__) || defined(__WINDOWS__) // Windows
#define WIN32
#elseif !defined(__linux__) && (defined(__linux)) || defined(linux) || defined(__unix) || defined(__unix__) //linux
#define __linux__
#endif


#if defined __linux__
#if defined NO_GLEW

#include <GL/glx.h>
#include <GL/gl.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#else // NO_GLEW

#include <GL/glxew.h>
#include <GL/glx.h>
#include <X11/XKBlib.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#endif // NO_GLEW

#define _E_THREAD_EXIT(x) pthread_exit( x );

#endif // __linux__

#if defined WIN32
#define E_COLOR_DISABLED
#define E_COLOR_NO_TERMTEST
#endif // WIN32


/*!
 * \namespace e_engine
 * \brief The main namespace of this OpenGL engine
 *
 * This is the main namespace of this project, which reduces
 * the nameincombabillitys of the project
 */
namespace e_engine {

// Keys
#define E_KEY_UNKNOWN      -1
#define E_KEY_SPACE        32
#define E_KEY_SPECIAL      256

#define E_KEY_BACKSPACE    ( E_KEY_SPECIAL + 1 )
#define E_KEY_TAB          ( E_KEY_SPECIAL + 2 )
#define E_KEY_CLEAR        ( E_KEY_SPECIAL + 3 )
#define E_KEY_RETURN       ( E_KEY_SPECIAL + 4 )
#define E_KEY_PAUSE        ( E_KEY_SPECIAL + 5 )
#define E_KEY_SYS_REQ      ( E_KEY_SPECIAL + 6 )
#define E_KEY_ESCAPE       ( E_KEY_SPECIAL + 7 )
#define E_KEY_DELETE       ( E_KEY_SPECIAL + 8 )

#define E_KEY_CAPS_LOCK    ( E_KEY_SPECIAL + 9 )
#define E_KEY_SCROLL_LOCK  ( E_KEY_SPECIAL + 10 )

#define E_KEY_F1           ( E_KEY_SPECIAL + 11 )
#define E_KEY_F2           ( E_KEY_SPECIAL + 12 )
#define E_KEY_F3           ( E_KEY_SPECIAL + 13 )
#define E_KEY_F4           ( E_KEY_SPECIAL + 14 )
#define E_KEY_F5           ( E_KEY_SPECIAL + 15 )
#define E_KEY_F6           ( E_KEY_SPECIAL + 16 )
#define E_KEY_F7           ( E_KEY_SPECIAL + 17 )
#define E_KEY_F8           ( E_KEY_SPECIAL + 18 )
#define E_KEY_F9           ( E_KEY_SPECIAL + 19 )
#define E_KEY_F10          ( E_KEY_SPECIAL + 20 )
#define E_KEY_F11          ( E_KEY_SPECIAL + 21 )
#define E_KEY_F12          ( E_KEY_SPECIAL + 22 )
#define E_KEY_F13          ( E_KEY_SPECIAL + 23 )
#define E_KEY_F14          ( E_KEY_SPECIAL + 24 )
#define E_KEY_F15          ( E_KEY_SPECIAL + 25 )
#define E_KEY_F16          ( E_KEY_SPECIAL + 26 )
#define E_KEY_F17          ( E_KEY_SPECIAL + 27 )
#define E_KEY_F18          ( E_KEY_SPECIAL + 28 )
#define E_KEY_F19          ( E_KEY_SPECIAL + 29 )
#define E_KEY_F20          ( E_KEY_SPECIAL + 30 )
#define E_KEY_F21          ( E_KEY_SPECIAL + 31 )
#define E_KEY_F22          ( E_KEY_SPECIAL + 32 )
#define E_KEY_F23          ( E_KEY_SPECIAL + 33 )
#define E_KEY_F24          ( E_KEY_SPECIAL + 34 )
#define E_KEY_F25          ( E_KEY_SPECIAL + 35 )

#define E_KEY_UP           ( E_KEY_SPECIAL + 36 )
#define E_KEY_DOWN         ( E_KEY_SPECIAL + 37 )
#define E_KEY_LEFT         ( E_KEY_SPECIAL + 38 )
#define E_KEY_RIGHT        ( E_KEY_SPECIAL + 39 )

#define E_KEY_L_SHIFT      ( E_KEY_SPECIAL + 40 )
#define E_KEY_R_SHIFT      ( E_KEY_SPECIAL + 41 )
#define E_KEY_L_CTRL       ( E_KEY_SPECIAL + 42 )
#define E_KEY_R_CTRL       ( E_KEY_SPECIAL + 43 )
#define E_KEY_L_ALT        ( E_KEY_SPECIAL + 44 )
#define E_KEY_R_ALT        ( E_KEY_SPECIAL + 45 )
#define E_KEY_L_SUPER      ( E_KEY_SPECIAL + 46 )
#define E_KEY_R_SUPER      ( E_KEY_SPECIAL + 47 )

#define E_KEY_KP_0         ( E_KEY_SPECIAL + 48 )
#define E_KEY_KP_1         ( E_KEY_SPECIAL + 49 )
#define E_KEY_KP_2         ( E_KEY_SPECIAL + 50 )
#define E_KEY_KP_3         ( E_KEY_SPECIAL + 51 )
#define E_KEY_KP_4         ( E_KEY_SPECIAL + 52 )
#define E_KEY_KP_5         ( E_KEY_SPECIAL + 53 )
#define E_KEY_KP_6         ( E_KEY_SPECIAL + 54 )
#define E_KEY_KP_7         ( E_KEY_SPECIAL + 55 )
#define E_KEY_KP_8         ( E_KEY_SPECIAL + 56 )
#define E_KEY_KP_9         ( E_KEY_SPECIAL + 57 )

#define E_KEY_HOME         ( E_KEY_SPECIAL + 58 )
#define E_KEY_INSERT       ( E_KEY_SPECIAL + 59 )
#define E_KEY_PAGE_UP      ( E_KEY_SPECIAL + 60 )
#define E_KEY_PAGE_DOWN    ( E_KEY_SPECIAL + 61 )
#define E_KEY_END          ( E_KEY_SPECIAL + 62 )
#define E_KEY_MENU         ( E_KEY_SPECIAL + 63 )

#define E_KEY_KP_DIVIDE    ( E_KEY_SPECIAL + 64 )
#define E_KEY_KP_MULTIPLY  ( E_KEY_SPECIAL + 65 )
#define E_KEY_KP_SUBTRACT  ( E_KEY_SPECIAL + 66 )
#define E_KEY_KP_ADD       ( E_KEY_SPECIAL + 67 )
#define E_KEY_KP_DECIMAL   ( E_KEY_SPECIAL + 68 )
#define E_KEY_KP_EQUAL     ( E_KEY_SPECIAL + 69 )
#define E_KEY_KP_ENTER     ( E_KEY_SPECIAL + 70 )
#define E_KEY_KP_NUM_LOCK  ( E_KEY_SPECIAL + 71 )

#define E_KEY_ENTER        E_KEY_RETURN
#define _E_KEY_LAST        E_KEY_KP_NUM_LOCK

#define E_KEY_PRESSED      1
#define E_KEY_RELEASED     0

}


#endif // PLATFORM_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
