/*!
 * \file windows/context.hpp
 * \brief \b Classes: \a eContext
 *
 * This file contains the class \b eContext which creates
 * the window in Windows and the OpenGL context on it.
 *
 * \sa e_context.cpp e_eInit.cpp
 */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "defines.hpp"
#include "displays.hpp"
#include "log.hpp"
#include "eInitEventBasic.hpp"

#include <GL/glew.h>
#include <GL/wglew.h>

namespace e_engine {

namespace windows_win32 {

class eContext : public eInitEventBasic {
   private:
      PIXELFORMATDESCRIPTOR vPixelFormat_PFD;
      HINSTANCE             vInstance_win32;
      WNDCLASS              vWindowClass_win32;
      HWND                  vHWND_Window_win32;
      RECT                  vWindowRect_win32;
      HDC                   vHDC_win32;
      HGLRC                 vOpenGLContext_WGL;
      LPCSTR                vClassName_win32;

      static LRESULT CALLBACK initialWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
      static LRESULT CALLBACK staticWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
      LRESULT CALLBACK        actualWndProc( UINT _uMsg, WPARAM _wParam, LPARAM _lParam, eWinInfo _tempInfo );

      bool                  vWindowsCallbacksError_B;

      bool                  vHasContext_B;
      bool                  vHasGLEW_B;

      virtual void makeEInitEventBasicAbstract() {}

   protected:
      bool                  vWindowRecreate_B;
      
      bool                  vWindowsDestroy_B;
      bool                  vWindowsNCDestrox_B;
      
      HWND getHWND_win32() {return vHWND_Window_win32;}

   public:
      eContext();

      inline void swapBuffers() { SwapBuffers( vHDC_win32 ); }

      int  createContext();

      int  fullScreen( ACTION _action, bool _allMonitors = false );
      int  enableVSync();
      int  disableVSync();
      void destroyContext();
      bool getHaveContext() { return vHasContext_B; }

      bool makeContextCurrent();
      bool makeNOContextCurrent();

      bool setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 = NONE ) {return false;}

      std::vector<eDisplays> getDisplayResolutions() { return std::vector<eDisplays>(); }

      bool setDisplaySizes( eDisplays const &_disp ) {return false;}
      void setPrimary( eDisplays const &_disp ) {}

      bool applyNewRandRSettings() {return false;}

      int  setFullScreenMonitor( eDisplays _disp ) {return 0;}
      bool setDecoration( ACTION _action );
      int  changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY ) {return 0;}
      bool fullScreenMultiMonitor() {return false;}
};

namespace e_engine_internal {

class eWindowClassRegister {
   private:
      bool vClass1Registered;
      bool vClass2Registered;
   public:
      eWindowClassRegister() : vClass1Registered( false ), vClass2Registered( false ) {}

      bool getC1() { return vClass1Registered; }
      bool getC2() { return vClass2Registered; }

   private:
      void setC1() { vClass1Registered = true; }
      void setC2() { vClass2Registered = true; }

      friend class e_engine::windows_win32::eContext;
};

extern eWindowClassRegister CLASS_REGISTER;

} // e_engine_internal

} // windows_win32

} // e_engine


#endif // CONTEXT_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
