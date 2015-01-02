/*!
 * \file x11/iContext.hpp
 * \brief \b Classes: \a iContext
 *
 * This file contains the class \b iContext which creates
 * the window and the OpenGL context on it.
 *
 * \sa e_context.cpp e_iInit.cpp
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

#ifndef E_CONTEXT_HPP
#define E_CONTEXT_HPP

#include "defines.hpp"

#include "iRandR.hpp"
#include "iKeyboard.hpp"
#include <GL/glew.h>
// WARNING Can not include <glxew.h> because it would overwrite all <glx.h> macros
//         which dont work before calling glewInit();
#include <GL/glx.h>


namespace e_engine {

namespace unix_x11 {

typedef GLXContext ( *glXCreateContextAttribsARBProc )(
      Display *, GLXFBConfig, GLXContext, bool, const int * ); // <= Old Style
// using glXCreateContextAttribsARBProc = GLXContext( * )( Display *, GLXFBConfig, GLXContext, bool,
// const int * );   // <= C++11 style (doesnt work with older gcc versions)


/*!
 * \brief Handles the 'user presses X button' event
 *
 * Without defining an event for the X button X11 will
 * force close the window and cause errors
 */
extern Atom atom_wmDeleteWindow;

/*!
 * \class e_engine::iContext
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
 * writing in one ( iContext_window.cpp ) all functions
 * called \b before \c GLEW is init and needing \c GLX
 * functions and in the other file ( iContext.cpp ) the
 * rest of the functions.
 */
class iContext : public iRandR, public iKeyboard {
 private:
   // X11 variables
   Display *vDisplay_X11;        //!< The connection to the X-Server
   Window vWindow_X11;           //!< The window handle
   Window vRootWindow_X11;       //!< The root window handle
   int vScreen_X11;              //!< The screen handel
   XVisualInfo *vVisualInfo_X11; //!< A structure containing information about the visual
   int vWindowMask_X11;          //!< The Window mask
   XSetWindowAttributes vWindowAttributes_X11; //!< A window attribute structure
   XTextProperty vWindowNameTP_X11;            //!< The window name text property
   XTextProperty vWindowIconTP_X11;            //!< The window icon text property
   XSizeHints *vSizeHints_X11;                 //!< The window size WM hints structure
   XWMHints *vWmHints_X11;                     //!< The WM hints structure
   XClassHint *vClassHints_X11;                //!< The class hints structure
   Colormap vColorMap_X11;                     //!< The clormap handle
   GLXContext vOpenGLContext_GLX;              //!< The context handle
   GLXFBConfig *vFBConfig_GLX;                 //!< The framebuffer handle
   int vNumOfFBConfigs_I;                      //!< Number of found matching framebuffer configs
   long int vEventMask_lI;                     //!< The X11 event mask (needed to recieve events)

   glXCreateContextAttribsARBProc
         glXCreateContextAttribsARB; //! The context creation function pointer


   GLuint vVertexArray_OGL;

   int vBestFBConfig_I; //!< The Integer ID of the best FB config we have found

   bool vWindowHasBorder_B;

   bool vHaveContext_B;
   bool vWindowCreated_B;
   bool vColorMapCreated_B;
   bool vDisplayCreated_B;
   bool vHaveGLEW_B;

   int vGLXVersionMajor_I;
   int vGLXVersionMinor_I;
   int vX11VersionMajor_I;
   int vX11VersionMinor_I;

   bool vIsMouseGrabbed_B;
   bool vIsCursorHidden_B;

   bool
   isExtensionSupported( const char *_extension ); //!< Function checking if extension is supported

   //! \todo create a function for setting an icon

   // bool createIconPixmap();  //! Does NOT work until now
   // void pixmaps2(unsigned int Width, unsigned int Height, const Uint8* Pixels);

   int createDisplay(); //!< Creates the connection to the X-Server \returns \c SUCCESS: \a 1 -- \c
   // ERRORS: \a -1 , \a -2
   int createFrameBuffer(); //!< Looks for the best FB config           \returns \c SUCCESS: \a 1 --
                            //\c ERRORS: \a -3
   int createWindow(); //!< Creates the Window                     \returns \c SUCCESS: \a 1 -- \c
   // ERRORS: \a -4
   int createOGLContext(); //!< Creates the OpenGL context             \returns \c SUCCESS: \a 1 --
                           //\c ERRORS: \a 3

   bool sendX11Event( std::string _atom,
                      GLint64 _l0 = 0,
                      GLint64 _l1 = 0,
                      GLint64 _l2 = 0,
                      GLint64 _l3 = 0,
                      GLint64 _l4 = 0 );

 protected:
   bool vWindowRecreate_B;

 public:
   iContext();
   virtual ~iContext() { destroyContext(); }

   int createContext();


   void getX11Version( int *_major, int *_minor );
   void getGLXVersion( int *_major, int *_minor );

   Display *getDisplay() {
      return vDisplay_X11;
   } //!< \brief Get the display pointer         \returns The display pointer
   Window const &getWindow() const {
      return vWindow_X11;
   } //!< \brief Get the window handle          \returns The window handle
   GLXContext const &getContext() const {
      return vOpenGLContext_GLX;
   } //!< \brief Get the context handle         \returns The context handle
   bool const &getHaveGLEW() const {
      return vHaveGLEW_B;
   } //!< \brief Check if GLEW is OK             \returns Whether GLEW is OK
   bool const &getHaveContext() const {
      return vHaveContext_B;
   } //!< \brief Check if we have a OGL context \returns If there is a OpenGL context

   inline void swapBuffers() {
      glXSwapBuffers( vDisplay_X11, vWindow_X11 );
   } //!< Swaps the OGL buffers


   bool makeContextCurrent();
   bool makeNOContextCurrent();

   static bool isAContextCurrentForThisThread();


   void destroyContext();

   int enableVSync();
   int disableVSync();

   int changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY );

   bool setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 = NONE );

   bool fullScreen( ACTION _action, bool _allMonitors = false );
   bool fullScreenMultiMonitor();
   int setFullScreenMonitor( iDisplays _disp );
   bool maximize( ACTION _action );
   bool setDecoration( ACTION _action );

   bool grabMouse();
   bool freeMouse();
   bool getIsMouseGrabbed() const;

   bool moveMouse( unsigned int _posX, unsigned int _posY );

   bool hideMouseCursor();
   bool showMouseCursor();
   bool getIsCursorHidden() const;


   //       GLuint getVertexArrayOpenGL() { return vVertexArray_OGL; }
};


} // unix_x11

} // e_engine



#endif // E_CONTEXT_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
