/*!
 * \file uConfig.cpp
 * \brief \b Classes: \a uConfig
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

#include "defines.hpp"

#if UNIX_X11
#include <GL/glxew.h>
#endif

#if WINDOWS
#include <GL/glew.h>
#include <GL/wglext.h>
#endif


#include "uConfig.hpp"

using std::string;

namespace e_engine {


_uConfig::__uConfig_FBA::__uConfig_FBA() {
   reset();
}

_uConfig::__uConfig_Versions::__uConfig_Versions() {
   reset();
}

_uConfig::__uConfig_Window::__uConfig_Window() {
   reset();
}

_uConfig::__uLogData_Config::__uLogData_Config() {
   reset();
}

_uConfig::__uConfig_Config::__uConfig_Config() {
   reset();
}



void _uConfig::__uConfig_FBA::reset() {
#if  UNIX
   FBA_RENDER_TYPE           = GLX_RGBA_BIT;
   FBA_RENDERABLE            = GL_TRUE;
   FBA_DRAWABLE_TYPE         = GLX_WINDOW_BIT;
   FBA_DOUBLEBUFFER          = GL_TRUE;
#endif // UNIX
   FBA_RED                   = 8;  // 8
   FBA_GREEN                 = 8;  // 8
   FBA_BLUE                  = 8;  // 8
   FBA_ALPHA                 = 8;  // 8
   FBA_DEPTH                 = 24; // 24
   FBA_STENCIL               = 8;  // 8
#if UNIX
   FBA_VISUAL_TYPE           = GLX_TRUE_COLOR;
#endif
   //FBA_STEREO                = GL_TRUE;
   
#if WINDOWS
   FBA_DRAW_TO_WINDOW        = 1;
   FBA_ACCELERATION          = WGL_FULL_ACCELERATION_ARB;
   FBA_RENDER_TYPE           = WGL_TYPE_RGBA_ARB;
#endif // WINDOWS
}

void _uConfig::__uConfig_Versions::reset() {
   minGlxMajorVer            = 1;
   minGlxMinorVer            = 3;
   glMajorVersion            = 4;
   glMinorVersion            = 5;
}

void _uConfig::__uConfig_Window::reset() {
   width                     = 800;
   height                    = 600;
   minWidth                  = 50;
   minHeight                 = 50;
   posX                      = 0;
   posY                      = 0;
   
   mousePosX                 = 0;
   mousePosY                 = 0;
   
   mouseIsInWindow           = false;
   windowHasFocus            = true;

   fullscreen                = false;
   VSync                     = true;
   windowDecoration          = true;
   
   winType                   = NORMAL;

   windowName                = "E Engine Window";
   xlibWindowName            = "E Engine Window";
   iconName                  = "E Engine Window";
   
   restoreOldScreenRes       = true;
   //iconPath.clear();
}

void _uConfig::__uLogData_Config::reset() {
   standardTimeColor        = 'M';
   
   maxFilenameSize          = 20;
   maxFunctionNameSize      = 25;
   
   standardShowTime         = true;
   standardShowFile         = true;
   standardShowLine         = true;
   
   standardWarningsToStdErr = false;
   
   logDefaultInit           = true;
   
   useHistory               = true;
   
   waitUntilLogEntryPrinted = false;
   
   width                    = -1;
   
   logOUT.colors            = DISABLED;
   logOUT.Time              = LEFT_REDUCED;
   logOUT.File              = RIGHT_REDUCED;
   logOUT.ErrorType         = LEFT_FULL;
   
   logERR.colors            = REDUCED;
   logERR.Time              = LEFT_REDUCED;
   logERR.File              = RIGHT_REDUCED;
   logERR.ErrorType         = LEFT_FULL;
   
   logFILE.Time             = LEFT_FULL;
   logFILE.File             = LEFT_FULL;
   logFILE.ErrorType        = LEFT_FULL;
   logFILE.logFileName.clear();
}


void _uConfig::__uConfig_Config::reset() {
   appName = "e-engine";
   configSubFolder.clear();
   logSubFolder = "log";

   useTimeAtCMD = false;
   useTimeAtLog = true;

   useCMDColor  = true;

   unixPathType = true;

   maxNumOfLogFileBackshift = 10;
}



_uConfig::_uConfig() {
   handleSIGINT                  = true;
   handleSIGTERM                 = true;
   timeoutForMainLoopThread_mSec = 1000; // 1 second

}

_uConfig GlobConf;

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
