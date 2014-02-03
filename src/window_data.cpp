/// \file window_data.cpp
/// \brief \b Classes: \a eWindowData
/// \sa e_window_data.hpp
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



#include "window_data.hpp"

using std::string;

namespace e_engine {

namespace e_engine_internal {

__eWindowData_FBA::__eWindowData_FBA() {
   reset();
}

__eWindowData_Versions::__eWindowData_Versions() {
   reset();
}

__eWindowData_Window::__eWindowData_Window() {
   reset();
}

__eLogData_Config::__eLogData_Config() {
   reset();
}


__eWindowData_Config::__eWindowData_Config() {
   reset();
}



void __eWindowData_FBA::reset() {
#if  UNIX_X11
   FBA_RENDER_TYPE           = GLX_RGBA_BIT;
   FBA_RENDERABLE            = GL_TRUE;
   FBA_DRAWABLE_TYPE         = GLX_WINDOW_BIT;
   FBA_DOUBLEBUFFER          = GL_TRUE;
#endif // UNIX_X11
   FBA_RED                   = 8;  // 8
   FBA_GREEN                 = 8;  // 8
   FBA_BLUE                  = 8;  // 8
   FBA_ALPHA                 = 8;  // 8
   FBA_DEPTH                 = 24; // 24
   FBA_STENCIL               = 8;  // 8
#if UNIX_X11
   FBA_VISUAL_TYPE           = GLX_TRUE_COLOR;
#endif
   //FBA_STEREO                = GL_TRUE;
}

void __eWindowData_Versions::reset() {
   minGlxMajorVer            = 1;
   minGlxMinorVer            = 3;
   glMajorVersion            = 4;
   glMinorVersion            = 3;
}

void __eWindowData_Window::reset() {
   width                     = 800;
   height                    = 600;
   minWidth                  = 50;
   minHeight                 = 50;
   posX                      = 0;
   posY                      = 0;

   fullscreen                = false;
   VSync                     = true;
   
   winType                   = NORMAL;

   windowName                = "E Engine Window";
   xlibWindowName            = "E Engine Window";
   iconName                  = "E Engine Window";
   
   restoreOldScreenRes       = true;
   //iconPath.clear();
}

void __eLogData_Config::reset() {
   standardTimeColor        = 'M';
   
   maxFilenameSize          = 20;
   
   standardShowTime         = true;
   standardShowFile         = true;
   standardShowLine         = true;
   
   standardWarningsToStdErr = false;
   
   logDefaultInit           = true;
   
   useHistory               = true;
   
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


void __eWindowData_Config::reset() {
   appName = "e-engine";
   configSubFolder.clear();
   logSubFolder = "log";

   useTimeAtCMD = false;
   useTimeAtLog = true;

   useCMDColor  = true;

   unixPathType = true;

   maxNumOfLogFileBackshift = 10;
}


}


_eWindowData::_eWindowData() {

   handleSIGINT                  = true;
   handleSIGTERM                 = true;
   timeoutForMainLoopThread_mSec = 1000; // 1 second

}

_eWindowData WinData;

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
