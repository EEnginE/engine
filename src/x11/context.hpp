/*! 
 * \file x11/context.hpp
 * \brief \b Classes: \a eContext
 * 
 * This file contains the class \b eContext which creates
 * the window and the OpenGL context on it.
 * 
 * \sa e_context.cpp e_eInit.cpp
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

#ifndef E_CONTEXT_HPP
#define E_CONTEXT_HPP


#include "randr.hpp"
#include "defines.hpp"
#include <GL/glew.h>
// WARNING Can not include <glxew.h> because it would overwrite all <glx.h> macros
//         which dont work before calling glewInit();
#include <GL/glx.h>


namespace e_engine {

namespace unix_x11 {

typedef GLXContext( *glXCreateContextAttribsARBProc )( Display *, GLXFBConfig, GLXContext, bool, const int * );  // <= Old Style
// using glXCreateContextAttribsARBProc = GLXContext( * )( Display *, GLXFBConfig, GLXContext, bool, const int * );   // <= C++11 style (doesnt work with older gcc versions)


/*!
 * \brief Handles the 'user presses X button' event
 *
 * Without defining an event for the X button X11 will
 * force close the window and cause errors
 */
extern Atom atom_wmDeleteWindow;

/*!
 * \class e_engine::eContext
 * \brief The OpenGL context creation class
 *
 * This class creates the connection to the X-Server,
 * opens a window and creates a drawable OpenGL
 * context.
 *
 * \par GLEW chaos
 *
 * \par
 * The definitions of the functions executed before \c GLEW
 * is initiated are int the source file e_context_window.cpp and
 * the others in the file e_context.cpp because some functions
 * need \c GLX extensions, which are in the file glxew.h. All
 * functions there will be init when glewInit(); is called.
 *
 * \par
 * Unfortunately glxew.h overwrites all functions from glx.h,
 * even those which are not extensions but normal functions.
 * So these functions can only be called \b after \c GLEW is
 * init, but \c GLEW needs a valid OpenGL context to start.
 * This causes that if you include glxew.h, functions which
 * are needed to create the context can not be called before
 * \c GLEW is init, which needs this context.
 *
 * \par
 * I solved this problem by creating \a 2 .cpp files and
 * writing in one ( e_context_window.cpp ) all functions
 * called \b before \c GLEW is init and needing \c GLX
 * functions and in the other file ( e_context.cpp ) the
 * rest of the functions.
 */
class eContext : public eRandR {
   private:

      // X11 variables
      Display              *vDisplay_X11;             //!< The connection to the X-Server
      Window               vWindow_X11;               //!< The window handle
      Window               vRootWindow_X11;           //!< The root window handle
      int                  vScreen_X11;               //!< The screen handel
      XVisualInfo          *vVisualInfo_X11;          //!< A structure containing information about the visual
      int                  vWindowMask_X11;           //!< The Window mask
      XSetWindowAttributes vWindowAttributes_X11;     //!< A window attribute structure
      XTextProperty        vWindowNameTP_X11;         //!< The window name text property
      XTextProperty        vWindowIconTP_X11;         //!< The window icon text property
      XSizeHints           *vSizeHints_X11;           //!< The window size WM hints structure
      XWMHints             *vWmHints_X11;             //!< The WM hints structure
      XClassHint           *vClassHints_X11;          //!< The class hints structure
      Colormap             vColorMap_X11;             //!< The clormap handle
      GLXContext           vOpenGLContext_GLX;        //!< The context handle
      GLXFBConfig          *vFBConfig_GLX;            //!< The framebuffer handle
      int                  vNumOfFBConfigs_I;         //!< Number of found matching framebuffer configs

      int    vBestFBConfig_I;

      bool   vWindowHasBorder_B;

      bool   vHaveContext_B;
      bool   vWindowCreated_B;
      bool   vColorMapCreated_B;
      bool   vDisplayCreated_B;
      bool   vHaveGLEW_B;

      int    vGLXVersionMajor_I;
      int    vGLXVersionMinor_I;
      int    vX11VersionMajor_I;
      int    vX11VersionMinor_I;

      bool   isExtensionSupported( const char *_extension ); //!< Function checking if extension is supported

      //! \todo create a function for setting an icon

      //bool createIconPixmap();  //! Does NOT work until now
      //void pixmaps2(unsigned int Width, unsigned int Height, const Uint8* Pixels);

      int createDisplay();        //!< Creates the connection to the X-Server \returns \c SUCCESS: \a 1 -- \c ERRORS: \a -1 , \a -2
      int createFrameBuffer();    //!< Looks for the best FB config           \returns \c SUCCESS: \a 1 -- \c ERRORS: \a -3
      int createWindow();         //!< Creates the Window                     \returns \c SUCCESS: \a 1 -- \c ERRORS: \a -4
      int createOGLContext();     //!< Creates the OpenGL context             \returns \c SUCCESS: \a 1 -- \c ERRORS: \a 3
      
      bool sendX11Event( std::string _atom, GLint64 _l0 = 0, GLint64 _l1 = 0, GLint64 _l2 = 0, GLint64 _l3 = 0, GLint64 _l4 = 0 );

   public:
      eContext() {
         vNumOfFBConfigs_I      = 0;
         vDisplay_X11           = NULL;
         vWindow_X11            = 0;
         vWindowHasBorder_B     = true;
         vHaveContext_B         = false;
         vDisplayCreated_B      = false;
         vWindowCreated_B       = false;
         vColorMapCreated_B     = false;
      }
      virtual ~eContext() {destroyContext();}

      int createContext();


      void       getX11Version( int *_major, int *_minor );
      void       getGLXVersion( int *_major, int *_minor );

      Display   *getDisplay()     { return  vDisplay_X11; }        //!< \brief Get the display pointer         \returns The display pointer
      Window     getWindow()      { return  vWindow_X11; }         //!< \brief Get the window handle          \returns The window handle
      GLXContext getContext()     { return  vOpenGLContext_GLX; }  //!< \brief Get the context handle         \returns The context handle
      bool       getHaveGLEW()    { return  vHaveGLEW_B; }         //!< \brief Check if GLEW is OK             \returns Whether GLEW is OK
      bool       getHaveContext() { return  vHaveContext_B; }      //!< \brief Check if we have a OGL context \returns If there is a OpenGL context

      inline void swapBuffers() {glXSwapBuffers( vDisplay_X11, vWindow_X11 );} //!< Swaps the OGL buffers


      bool makeContextCurrent();
      bool makeNOContexCurrent();


      void destroyContext();

      int  enableVSync();
      int  disableVSync();

      int  changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY );

      bool setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 = NONE );

      bool fullScreen( ACTION _action, bool _allMonitors = false );
      bool fullScreenMultiMonitor();
      int  setFullScreenMonitor( eDisplays _disp );
      bool maximize( ACTION _action );
      bool setDecoration( ACTION _action );
};


} // unix_x11

} // e_engine



#endif // E_CONTEXT_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

