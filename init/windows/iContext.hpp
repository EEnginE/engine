/*!
 * \file windows/iContext.hpp
 * \brief \b Classes: \a iContext
 *
 * This file contains the class \b iContext which creates
 * the window in Windows and the OpenGL context on it.
 *
 * \sa e_context.cpp e_iInit.cpp
 */

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "defines.hpp"
#include "iInitEventBasic.hpp"
#include "iKeyboard.hpp"
#include "iRandR.hpp"

#include <GL/glew.h>
#include <GL/wglew.h>

namespace e_engine {

namespace windows_win32 {


class iContext : public iInitEventBasic, public iKeyboard, public iRandR {
   private:
      PIXELFORMATDESCRIPTOR vPixelFormat_PFD;
      HINSTANCE             vInstance_win32;
      WNDCLASSW             vWindowClass_win32;
      HWND                  vHWND_Window_win32;
      RECT                  vWindowRect_win32;
      HDC                   vHDC_win32;
      HGLRC                 vOpenGLContext_WGL;
      LPCWSTR               vClassName_win32;

      static LRESULT CALLBACK initialWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
      static LRESULT CALLBACK staticWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
      LRESULT CALLBACK        actualWndProc( UINT _uMsg, WPARAM _wParam, LPARAM _lParam, iEventInfo _tempInfo );
      
      GLuint                vVertexArray_OGL;

      bool                  vWindowsCallbacksError_B;

      bool                  vHasContext_B;
      bool                  vHasGLEW_B;
      
      
      bool                  vIsMouseGrabbed_B;
      bool                  vIsCursorHidden_B;

      bool                  vAThreadOwnsTheOpenGLContext_B;

      bool                  setWindowState( UINT _flags, HWND _pos = ( HWND )1000 );

      virtual void makeEInitEventBasicAbstract() {}

   protected:
      bool                  vWindowRecreate_B;

      bool                  vWindowsDestroy_B;
      bool                  vWindowsNCDestrox_B;

      HWND getHWND_win32() {return vHWND_Window_win32;}
      
   public:
      iContext();
      virtual ~iContext() {if ( vHasContext_B ) destroyContext();}

      inline void swapBuffers() {SwapBuffers( vHDC_win32 );}

      int  createContext();

      int  fullScreen( ACTION _action, bool _allMonitors = false );
      int  enableVSync();
      int  disableVSync();
      void destroyContext();
      bool getHaveContext() { return vHasContext_B; }

      bool makeContextCurrent();
      bool makeNOContextCurrent();
      
      static bool isAContextCurrentForThisThread();

      bool setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 = NONE );


      int  setFullScreenMonitor( iDisplays _disp ) {return 0;}
      bool setDecoration( ACTION _action );
      int  changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY );
      bool fullScreenMultiMonitor() {return false;}

      bool grabMouse();              
      bool freiMouse();               
      bool getIsMouseGrabbed()  const; 

      bool moviMouse( unsigned int _posX, unsigned int _posY ); 

      bool hidiMouseCursor();         
      bool showMouseCursor();         
      bool getIsCursorHidden() const; 
      
//       GLuint getVertexArrayOpenGL() { return vVertexArray_OGL; }
};

/*!
 * \fn iContext::setFullScreenMonitor
 * \brief Not supported with Windows
 *
 * \note Does Nothing
 *
 * \todo Support more than one fullscreen monitor in Windows
 *
 * \returns 0
 */

/*!
 * \fn iContext::fullScreenMultiMonitor
 * \brief Not supported with Windows
 *
 * \note Does Nothing
 *
 * \todo Support more than one fullscreen monitor in Windows
 *
 * \returns false
 */

namespace e_engine_internal {

/*!
 * \brief Stores information about WIN32 window classes
 *
 * Window classes are registered globaly for one application.
 * Registering them more than once leads to errors. This class
 * stores if a window class is registered.
 */
class eWindowClassRegister {
   private:
      bool vClass1Registered; //!< Is the temporary window class registered (used for the temporary OpenGL context)
      bool vClass2Registered; //!< Is the final window class registered (the "real" window class)
   public:
      //! Both window classes are not registered when the application starts
      eWindowClassRegister() : vClass1Registered( false ), vClass2Registered( false ) {}

      bool getC1() { return vClass1Registered; } //!< Is the temporary window class already registered?
      bool getC2() { return vClass2Registered; } //!< Is the final window class already registered?

   private:
      void setC1() { vClass1Registered = true; } //!< Set the temporary window class as registered (only windows_win32::iContext can do this)
      void setC2() { vClass2Registered = true; } //!< Set the final window class as registered (only windows_win32::iContext can do this)

      friend class e_engine::windows_win32::iContext;
};

//! Global object that stores the state of the window classes
extern eWindowClassRegister CLASS_REGISTER;

} // e_engine_internal

} // windows_win32

} // e_engine


#endif // CONTEXT_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
