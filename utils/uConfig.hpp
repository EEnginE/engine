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

namespace e_engine {

enum LOG_COLOR_TYPE { DISABLED, REDUCED, FULL };
enum LOG_PRINT_TYPE { OFF, LEFT_FULL, RIGHT_FULL, LEFT_REDUCED, RIGHT_REDUCED };
enum WINDOW_TYPE    {
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


namespace e_engine_internal {

}

/*!
 * \struct e_engine::_uConfig
 * \brief Structure for all important window informations
 *
 * This is the main, global configuration and information sructure.
 *
 * This structure stores all information the classes eInit and
 * eContext need.
 * The constructor fills all values with data,
 * which let the classes work without problems.
 *
 * Only variables like height, width and windowName should be changed.
 *
 * \note Only the object \c GlobConf will be used! You should never create other instances of this structure
 *
 * \sa eInit eContext GlobConf
 */
struct _uConfig {

   bool         handleSIGINT;  // Crt - C
   bool         handleSIGTERM;

   long int     timeoutForMainLoopThread_mSec;

   // =============================================================================
   // ==================================================================================================================
   // =======           ================================================================================================================
   // =====  Framebuffer  =================================================================================================================
   // =======           ================================================================================================================
   // ==================================================================================================================
   // =============================================================================

   struct __uConfig_FBA {
      int          FBA_RENDER_TYPE;
      int          FBA_RENDERABLE;
      int          FBA_DRAWABLE_TYPE;
      int          FBA_DOUBLEBUFFER;
      int          FBA_RED;
      int          FBA_GREEN;
      int          FBA_BLUE;
      int          FBA_ALPHA;
      int          FBA_DEPTH;
      int          FBA_STENCIL;
      int          FBA_VISUAL_TYPE;
      //int         FBA_STEREO;

      int          FBA_DRAW_TO_WINDOW;
      int          FBA_ACCELERATION;
      int          FBA_OGL_SUPPORTED;


      __uConfig_FBA();

      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } framebuffer;

   // =============================================================================
   // ==================================================================================================================
   // =======        ===================================================================================================================
   // =====  Versions  ====================================================================================================================
   // =======        ===================================================================================================================
   // ==================================================================================================================
   // =============================================================================

   struct __uConfig_Versions {
      int          minGlxMajorVer;
      int          minGlxMinorVer;
      int          glMajorVersion;
      int          glMinorVersion;

      __uConfig_Versions();

      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } versions;


   // =============================================================================
   // ==================================================================================================================
   // =======      =====================================================================================================================
   // =====  Window  ======================================================================================================================
   // =======      =====================================================================================================================
   // ==================================================================================================================
   // =============================================================================

   struct __uConfig_Window {
      unsigned int width;     //!< The width  ( will be updated when window changed ) \c CLASSES: \a eInit, \a eContext
      unsigned int height;    //!< The height ( will be updated when window changed ) \c CLASSES: \a eInit, \a eContext
      unsigned int minWidth;  //!< The min width of the window                        \c CLASSES: \a eInit, \a eContext
      unsigned int minHeight; //!< The min height of the window                       \c CLASSES: \a eInit, \a eContext
      int          posX;      //!< The posX   ( will be updated when window changed ) \c CLASSES: \a eInit, \a eContext
      int          posY;      //!< The posY   ( will be updated when window changed ) \c CLASSES: \a eInit, \a eContext
      
      unsigned int mousePosX; //!< The mouse posX   ( will be updated when mouse moved ) \c CLASSES: \a eInit, \a eContext
      unsigned int mousePosY; //!< The mouse posY   ( will be updated when mouse moved ) \c CLASSES: \a eInit, \a eContext
      
      bool         mouseIsInWindow; //!< Is the mouse in the window? ( will be updated when mouse moved ) \c CLASSES: \a eInit, \a eContext
      bool         windowHasFocus;  //!< Has our window the focus?   ( will be updated when mouse moved ) \c CLASSES: \a eInit, \a eContext
      
      

      bool         fullscreen;       //!< Fullscreen?          ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit
      bool         VSync;            //!< VSync?               ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit
      bool         windowDecoration; //!< Has a window border? ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit

      WINDOW_TYPE  winType;

      std::string  windowName;      //!< Name of the window ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit, \a eContext
      std::string  xlibWindowName;  //!< Name of the window ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit, \a eContext
      std::string  iconName;        //!< Name of the window ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit, \a eContext
      //std::string  iconPath;       //!< Name of the window ( changes will be ignored after eInit::init() called )

      bool         restoreOldScreenRes;

      __uConfig_Window();

      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } win;


   // =============================================================================
   // ==================================================================================================================
   // =======   ========================================================================================================================
   // =====  Log  =========================================================================================================================
   // =======   ========================================================================================================================
   // ==================================================================================================================
   // =============================================================================

   struct __uLogData_Config {
      char         standardTimeColor;

      unsigned int maxFilenameSize;
      unsigned int maxFunctionNameSize;

      bool         standardShowTime;
      bool         standardShowFile;
      bool         standardShowLine;

      bool         standardWarningsToStdErr;

      bool         logDefaultInit;

      bool         useHistory;

      bool         waitUntilLogEntryPrinted;

      int          width;                       //!< If width < 0, then the automatically determined size will be used

      struct __uLogDataStandardOut {
         LOG_COLOR_TYPE colors;
         LOG_PRINT_TYPE Time;
         LOG_PRINT_TYPE File;
         LOG_PRINT_TYPE ErrorType;
      } logOUT;

      struct __uLogDataErrorOut {
         LOG_COLOR_TYPE colors;
         LOG_PRINT_TYPE Time;
         LOG_PRINT_TYPE File;
         LOG_PRINT_TYPE ErrorType;
      } logERR;

      struct __uLogDataFileOut {
         LOG_PRINT_TYPE Time;
         LOG_PRINT_TYPE File;
         LOG_PRINT_TYPE ErrorType;

         std::string logFileName;   //!< The BASIC name (without .log, etc.) of the log file ( changes will be ignored after the 1st log entry ) \c CLASSES: \a uLog
      } logFILE;

      void reset();

      __uLogData_Config();
   } log;


   // =============================================================================
   // ==================================================================================================================
   // =======      =====================================================================================================================
   // =====  Config  ======================================================================================================================
   // =======      =====================================================================================================================
   // ==================================================================================================================
   // =============================================================================

   struct __uConfig_Config {
      std::string appName;          //!< The name of the program
      std::string configSubFolder;  //!< Sub folder in the main configuration folder ( clear for none )
      std::string logSubFolder;     //!< Sub folder in the main configuration folder ( clear for none ) ( changes will be ignored after the 1st log entry )

      bool useTimeAtCMD;            //!< Time on commandline when log entry starts. \c CLASSES: \a uLog
      bool useTimeAtLog;            //!< Time in logfile when log entry starts.     \c CLASSES: \a uLog

      bool useCMDColor;             //!< Turns all functions off \a eCMD useless if set \c false

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
      bool           unixPathType;

      __uConfig_Config();
      unsigned short maxNumOfLogFileBackshift;

      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } config;

   // Versions


   _uConfig();
   void useAutoOpenGLVersion() {versions.glMajorVersion = -5; versions.glMinorVersion = -5;}
};

/*!
 * \brief The main object from _uConfig
 * \sa _uConfig __uConfig_FBA
 */
extern _uConfig GlobConf;

}

#endif // U_CONFIG_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
