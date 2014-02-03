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
};

}


#endif // CONTEXT_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
