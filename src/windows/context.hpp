/*!
 * \file context.hpp
 * \brief \b Classes: \a eContext
 *
 * This file contains the class \b eContext which creates
 * the window in Windows and the OpenGL context on it.
 *
 * \sa e_context.cpp e_init.cpp
 */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "defines.hpp"
#include "displays.hpp"

namespace e_engine {

class eContext {
   public:
      void swapBuffers() {}
      int  createContext() {return 1;}
      int  fullScreen( ACTION _action, bool _allMonitors = false ) {return 1;}
      int  enableVSync() {return 1;}
      void destroyContext() {}
      bool getHaveContext() { return true; }
      
      void makeContextCurrent()  {}
      void makeNOContexCurrent() {}
      
      bool setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 = NONE ) {return false;}
      
      std::vector<eDisplays> getDisplayResolutions() { return std::vector<eDisplays>(); }

      bool setDisplaySizes( eDisplays const &_disp ) {return false;}
      void setPrimary( eDisplays const &_disp ) {}

      bool applyNewRandRSettings() {return false;}
      
      int  setFullScreenMonitor( eDisplays _disp ) {return 0;}
      bool setDecoration( ACTION _action ) {return false;}
      int  changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY ) {return 0;}
      bool fullScreenMultiMonitor() {return false;}
};

}


#endif // CONTEXT_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
