/*!
 * \file uConfig.hpp
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

#ifndef U_CONFIG_HPP
#define U_CONFIG_HPP

#include <string>
#include <vector>
#include "uExtensions.hpp"

namespace e_engine {

enum LOG_COLOR_TYPE { DISABLED, REDUCED, FULL };
enum LOG_PRINT_TYPE { OFF, LEFT_FULL, RIGHT_FULL, LEFT_REDUCED, RIGHT_REDUCED };
enum WINDOW_TYPE {
   DESKTOP,
   DOCK,
   TOOLBAR,
   MENU,
   UTILITY,
   SPLASH,
   DIALOG,
   DROPDOWN_MENU,
   POPUP_MENU,
   TOOLTIP,
   NOTIFICATION,
   COMBO,
   DND,
   NORMAL
};

namespace internal {}

/*!
 * \struct e_engine::_uConfig
 * \brief Structure for all important window informations
 *
 * This is the main, global configuration and information sructure.
 *
 * This structure stores all information the classes iInit and
 * iContext need.
 * The constructor fills all values with data,
 * which let the classes work without problems.
 *
 * Only variables like height, width and windowName should be changed.
 *
 * \note Only the object \c GlobConf will be used! You should never create other
 * instances of this
 * structure
 *
 * \sa iInit iContext GlobConf
 */
struct _uConfig {

   bool handleSIGINT; // Crt - C
   bool handleSIGTERM;

   long int timeoutForMainLoopThread_mSec;

   //  ______                        _            __  __
   //  |  ___|                      | |          / _|/ _|
   //  | |_ _ __ __ _ _ __ ___   ___| |__  _   _| |_| |_ ___ _ __
   //  |  _| '__/ _` | '_ ` _ \ / _ \ '_ \| | | |  _|  _/ _ \ '__|
   //  | | | | | (_| | | | | | |  __/ |_) | |_| | | | ||  __/ |
   //  \_| |_|  \__,_|_| |_| |_|\___|_.__/ \__,_|_| |_| \___|_|
   //

   struct __uConfig_FBA {
      int FBA_RENDER_TYPE;
      int FBA_RENDERABLE;
      int FBA_DRAWABLE_TYPE;
      int FBA_DOUBLEBUFFER;
      int FBA_RED;
      int FBA_GREEN;
      int FBA_BLUE;
      int FBA_ALPHA;
      int FBA_DEPTH;
      int FBA_STENCIL;
      int FBA_VISUAL_TYPE;
      // int         FBA_STEREO;

      int FBA_DRAW_TO_WINDOW;
      int FBA_ACCELERATION;
      int FBA_OGL_SUPPORTED;

      __uConfig_FBA();

      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } framebuffer;

   //   _   _               _
   //  | | | |             (_)
   //  | | | | ___ _ __ ___ _  ___  _ __
   //  | | | |/ _ \ '__/ __| |/ _ \| '_ \
   //  \ \_/ /  __/ |  \__ \ | (_) | | | |
   //   \___/ \___|_|  |___/_|\___/|_| |_|
   //

   struct __uConfig_Versions {
      int minGlxMajorVer;
      int minGlxMinorVer;
      int glMajorVersion;
      int glMinorVersion;

      __uConfig_Versions();

      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } versions;

   //   _    _ _           _
   //  | |  | (_)         | |
   //  | |  | |_ _ __   __| | _____      __
   //  | |/\| | | '_ \ / _` |/ _ \ \ /\ / /
   //  \  /\  / | | | | (_| | (_) \ V  V /
   //   \/  \/|_|_| |_|\__,_|\___/ \_/\_/
   //

   struct __uConfig_Window {
      //! The width  ( will be updated when window changed ) \c CLASSES: \a iInit, \a iContext
      unsigned int width;

      //! The height ( will be updated when window changed ) \c CLASSES: \a iInit, \a iContext
      unsigned int height;

      //! The min width of the window                        \c CLASSES: \a iInit, \a iContext
      unsigned int minWidth;

      //! The min height of the window                       \c CLASSES: \a iInit, \a iContext
      unsigned int minHeight;

      //! The posX   ( will be updated when window changed ) \c CLASSES: \a iInit, \a iContext
      int posX;

      //! The posY   ( will be updated when window changed ) \c CLASSES: \a iInit, \a iContext
      int posY;

      //! The mouse posX (will be updated when mouse moved)  \c CLASSES: \a iInit, \a iContext
      unsigned int mousePosX;

      //! The mouse posY (will be updated when mouse moved)  \c CLASSES: \a iInit, \a iContext
      unsigned int mousePosY;

      //! Is the mouse in the window? (will be updated when mouse moved)
      bool mouseIsInWindow;

      //! Has our window the focus? (will be updated when mouse moved)
      bool windowHasFocus;

      //! Fullscreen? ( changes will be ignored after iInit::init() called ) \c CLASSES: \a iInit
      bool fullscreen;

      //! VSync? ( changes will be ignored after iInit::init() called ) \c CLASSES: \a iInit
      bool VSync;

      //! Has a window border? ( changes will be ignored after iInit::init() called )
      bool windowDecoration;

      WINDOW_TYPE winType;

      //! Name of the window (changes will be ignored after iInit::init() called)
      std::string windowName;

      //! Name of the window (changes will be ignored after iInit::init() called)
      std::string xlibWindowName;

      //! Name of the window (changes will be ignored after iInit::init() called)
      std::string iconName;

      bool restoreOldScreenRes;

      __uConfig_Window();

      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } win;

   //   _
   //  | |
   //  | |     ___   __ _
   //  | |    / _ \ / _` |
   //  | |___| (_) | (_| |
   //  \_____/\___/ \__, |
   //                __/ |
   //               |___/

   struct __uLogData_Config {
      char standardTimeColor;

      unsigned int maxFilenameSize;
      unsigned int maxFunctionNameSize;

      unsigned int threadNameWidth;

      bool standardShowTime;
      bool standardShowFile;
      bool standardShowLine;

      bool standardWarningsToStdErr;

      bool logDefaultInit;

      bool useHistory;

      bool waitUntilLogEntryPrinted;

      int width; //!< If width < 0, then the automatically determined size will be used

      struct __uLogDataStandardOut {
         LOG_COLOR_TYPE colors;
         LOG_PRINT_TYPE Time;
         LOG_PRINT_TYPE File;
         LOG_PRINT_TYPE ErrorType;
         LOG_PRINT_TYPE Thread;
      } logOUT;

      struct __uLogDataErrorOut {
         LOG_COLOR_TYPE colors;
         LOG_PRINT_TYPE Time;
         LOG_PRINT_TYPE File;
         LOG_PRINT_TYPE ErrorType;
         LOG_PRINT_TYPE Thread;
      } logERR;

      struct __uLogDataFileOut {
         LOG_PRINT_TYPE Time;
         LOG_PRINT_TYPE File;
         LOG_PRINT_TYPE ErrorType;
         LOG_PRINT_TYPE Thread;

         /*!
          * The BASIC name (without .log, etc.) of the log file ( changes will be ignored after
          * the 1st log entry ) \c CLASSES: \a uLog
          */
         std::string logFileName;
      } logFILE;

      void reset();

      __uLogData_Config();
   } log;

   //   _____              __ _
   //  /  __ \            / _(_)
   //  | /  \/ ___  _ __ | |_ _  __ _
   //  | |    / _ \| '_ \|  _| |/ _` |
   //  | \__/\ (_) | | | | | | | (_| |
   //   \____/\___/|_| |_|_| |_|\__, |
   //                            __/ |
   //                           |___/

   struct __uConfig_Config {
      std::string appName;         //!< The name of the program
      std::string configSubFolder; //!< Config sub dir (clear for none)
      std::string logSubFolder;    //!< Log sub dir (clear for none)

      bool useTimeAtCMD; //!< Time on commandline when log entry starts. \c CLASSES: \a uLog
      bool useTimeAtLog; //!< Time in logfile when log entry starts.     \c CLASSES: \a uLog

      bool useCMDColor; //!< Turns all functions off \a eCMD useless if set \c false

      /*!
       * \brief \c UNIX Main config path mode
       *
       * On \c UNIX like systems there are 2 locations for the
       * main config dir.
       *
       * The 1st is directly in the \a $HOME dir where the dir
       * name is the app name with a '.' ( \a $HOME/.e_engine )
       * To use this set the value to false
       *
       * The 2nd is the \a $HOME/.config folder where only the
       * name of the app is used for the folder (only if \a $HOME/.config exists)
       *
       */
      bool unixPathType;

      __uConfig_Config();
      unsigned short maxNumOfLogFileBackshift;

      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } config;

   struct __uConfig_OpenGL {
      /*!
       * 1: - force OLD shader query style - should always work
       * 2: - force NEW shader query style - even if the extension is not
       * supported ==> may cause
       * segfault
       * 0: Let rShader decide [default]
       */
      unsigned char shaderInfoQueryType;
      bool useShaders;

      __uConfig_OpenGL();
      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } ogl;

   //   _____
   //  /  __ \
   //  | /  \/ __ _ _ __ ___   ___ _ __ __ _
   //  | |    / _` | '_ ` _ \ / _ \ '__/ _` |
   //  | \__/\ (_| | | | | | |  __/ | | (_| |
   //   \____/\__,_|_| |_| |_|\___|_|  \__,_|
   //

   struct __uConfig_Camera {
      double mouseSensitivity; //!< Negative values will invert the mouse
      double movementSpeed;

      double angleHorizontal;
      double angleVertical;

      __uConfig_Camera();
      void reset();
   } camera;

   uExtensions extensions;

   // Versions

   _uConfig();
   void useAutoOpenGLVersion() {
      versions.glMajorVersion = -5;
      versions.glMinorVersion = -5;
   }
};

/*!
 * \brief The main object from _uConfig
 * \sa _uConfig __uConfig_FBA
 */
extern _uConfig GlobConf;
}

#endif // U_CONFIG_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
