/*!
 * \file defines.hpp
 *
 * Defines some variables
 *
 */

#ifndef DEFINES_HPP
#define DEFINES_HPP


#define UNIX_X11     @CM_UNIX_X11@
#define UNIX_WAYLAND @CM_UNIX_WAYLAND@
#define UNIX_MIR     @CM_UNIX_MIR@
#define WINDOWS      @CM_WINDOWS@

#if UNIX_X11 || UNIX_WAYLAND || UNIX_MIR
#define UNIX         1
#else
#define UNIX         0
#endif

// Select the OS namespace
#if UNIX_X11
#define OS_NAMESPACE unix_x11
#elif UNIX_WAYLAND
#define OS_NAMESPACE unix_wayland
#elif UNIX_MIR
#define OS_NAMESPACE unix_mir
#elif WINDOWS
#define OS_NAMESPACE windows_win32
#endif

#define E_VERSION_MAJOR     @CM_VERSION_MAJOR@
#define E_VERSION_MINOR     @CM_VERSION_MINOR@
#define E_VERSION_SUBMINOR  @CM_VERSION_SUBMINOR@
#define E_COMMIT_IS_TAGGED  @CM_COMMIT_IS_TAGGED@
#define E_VERSION_GIT      "@CM_VERSION_GIT@"
#define E_INSTALL_PREFIX   "@CMAKE_INSTALL_PREFIX@"


namespace e_engine {
   

/*!
* \brief Attributes for eContext::NET_WM_STATE()
* \sa eContext::NET_WM_STATE()
*/
enum WINDOW_ATTRIBUTE {
   MODAL,              //!< Sets the window modal
   STICKY,             //!< Sets the window sticky
   MAXIMIZED_VERT,     //!< Maximize the window in the vert. direction
   MAXIMIZED_HORZ,     //!< Maximize the window in the horz. direction
   SHADED,             //!< Should add some shadow effects
   SKIP_TASKBAR,       //!< Removes the windoe from the taskbar
   SKIP_PAGER,         //!< Removes the window from the pager
   HIDDEN,             //!< This should hide it somehow
   FULLSCREEN,         //!< This should make the window fullscreen
   ABOVE,              //!< This should set the window above all other windows (always in the foreground)
   BELOW,              //!< This should set the window below all other windows (always in the background)
   DEMANDS_ATTENTION,  //!< Tells the window manager to make the window borders blink or something like that -- stops usually when the user clicks in the window
   FOCUSED,            //!< Should set the window focused
   NONE                //!< Change nothing
};

/*!
 * \brief Actions for eContext::NET_WM_STATE()
 * \sa eContext::NET_WM_STATE()
 */
enum ACTION {
   C_REMOVE = 0, //!< Remove the attribute
   C_ADD,        //!< Add the attribute
   C_TOGGLE      //!< Toggle the attribute
};

}


// Keys
#define E_KEY_UNKNOWN      -1
#define E_KEY_SPACE        32
#define E_KEY_SPECIAL      256

#define E_MOUSE_BUTTON     0

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

//Mouse
#define E_MOUSE_LEFT      ( E_MOUSE_BUTTON + 0 )
#define E_MOUSE_MIDDLE    ( E_MOUSE_BUTTON + 1 )
#define E_MOUSE_RIGHT     ( E_MOUSE_BUTTON + 2 )
#define E_MOUSE_X         ( E_MOUSE_BUTTON + 3 )

#define _E_MOUSE_LAST     E_MOUSE_X

#define E_KEY_PRESSED      1
#define E_KEY_RELEASED     0

#endif // DEFINES_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
