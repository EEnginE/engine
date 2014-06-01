/*!
 * \file eWindowData.hpp
 * \brief \b Classes: \a eWindowData, \a eWinInfo
 * \sa e_eWindowData.cpp
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


#ifndef E_WINDOW_DATA_HPP
#define E_WINDOW_DATA_HPP

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
 * \struct e_engine::_eWindowData
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
 * \note Only the object \c WinData will be used! You should never create other instances of this structure
 *
 * \sa eInit eContext WinData
 */
struct _eWindowData {

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

   struct __eWindowData_FBA {
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
      

      __eWindowData_FBA();

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

   struct __eWindowData_Versions {
      int          minGlxMajorVer;
      int          minGlxMinorVer;
      int          glMajorVersion;
      int          glMinorVersion;

      __eWindowData_Versions();

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
   
   struct __eWindowData_Window {
      unsigned int width;     //!< The width  ( will be updated when window changed ) \c CLASSES: \a eInit, \a eContext
      unsigned int height;    //!< The height ( will be updated when window changed ) \c CLASSES: \a eInit, \a eContext
      unsigned int minWidth;  //!< The min width of the window                        \c CLASSES: \a eInit, \a eContext
      unsigned int minHeight; //!< The min height of the window                       \c CLASSES: \a eInit, \a eContext
      int          posX;      //!< The posX   ( will be updated when window changed ) \c CLASSES: \a eInit, \a eContext
      int          posY;      //!< The posY   ( will be updated when window changed ) \c CLASSES: \a eInit, \a eContext

      bool         fullscreen;       //!< Fullscreen?          ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit
      bool         VSync;            //!< VSync?               ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit
      bool         windowDecoration; //!< Has a window border? ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit

      WINDOW_TYPE  winType;

      std::string  windowName;      //!< Name of the window ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit, \a eContext
      std::string  xlibWindowName;  //!< Name of the window ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit, \a eContext
      std::string  iconName;        //!< Name of the window ( changes will be ignored after eInit::init() called ) \c CLASSES: \a eInit, \a eContext
      //std::string  iconPath;       //!< Name of the window ( changes will be ignored after eInit::init() called )

      bool         restoreOldScreenRes;

      __eWindowData_Window();

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

   struct __eLogData_Config {
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

      struct __eLogDataStandardOut {
         LOG_COLOR_TYPE colors;
         LOG_PRINT_TYPE Time;
         LOG_PRINT_TYPE File;
         LOG_PRINT_TYPE ErrorType;
      } logOUT;

      struct __eLogDataErrorOut {
         LOG_COLOR_TYPE colors;
         LOG_PRINT_TYPE Time;
         LOG_PRINT_TYPE File;
         LOG_PRINT_TYPE ErrorType;
      } logERR;

      struct __eLogDataFileOut {
         LOG_PRINT_TYPE Time;
         LOG_PRINT_TYPE File;
         LOG_PRINT_TYPE ErrorType;

         std::string logFileName;   //!< The BASIC name (without .log, etc.) of the log file ( changes will be ignored after the 1st log entry ) \c CLASSES: \a eLog
      } logFILE;

      void reset();

      __eLogData_Config();
   } log;


   // =============================================================================
   // ==================================================================================================================
   // =======      =====================================================================================================================
   // =====  Config  ======================================================================================================================
   // =======      =====================================================================================================================
   // ==================================================================================================================
   // =============================================================================
   
   struct __eWindowData_Config {
      std::string appName;          //!< The name of the program
      std::string configSubFolder;  //!< Sub folder in the main configuration folder ( clear for none )
      std::string logSubFolder;     //!< Sub folder in the main configuration folder ( clear for none ) ( changes will be ignored after the 1st log entry )

      bool useTimeAtCMD;            //!< Time on commandline when log entry starts. \c CLASSES: \a eLog
      bool useTimeAtLog;            //!< Time in logfile when log entry starts.     \c CLASSES: \a eLog

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

      __eWindowData_Config();
      unsigned short maxNumOfLogFileBackshift;

      /*!
       * \brief Reset to default
       * \returns Nothing
       */
      void reset();
   } config;

   // Versions


   _eWindowData();
   void useAutoOpenGLVersion() {versions.glMajorVersion = -5; versions.glMinorVersion = -5;}
};

/*!
 * \brief The main object from _eWindowData
 * \sa _eWindowData __eWindowData_FBA
 */
extern _eWindowData WinData;

class eInit;

/*!
 * \struct e_engine::eWinInfo
 * \brief Structure for evnet informations
 *
 *
 * \sa eInit eSignal eSlot
 */
struct eWinInfo {
   int   type;
   eInit *eInitPointer;

   /*!
    * \struct e_engine::eWinInfo::_eRsize
    * \brief The resize part
    */
   struct _eRsize {
      int          posX;
      int          posY;
      unsigned int height;
      unsigned int width;
   } eResize;

   /*!
    * \struct e_engine::eWinInfo::_eKey
    * \brief The key part
    */
   struct _eKey {
      wchar_t      key;
      unsigned int state;
   } eKey;

   /*!
    * \struct e_engine::eWinInfo::_eMouse
    * \brief The mouse part
    */
   struct _eMouse {
      int posX;
      int posY;
      int state;
   } eMouse;
   eWinInfo() {
      type = 0;
      eInitPointer = NULL;
      eResize.posX = 0;
      eResize.posY = 0;
      eResize.height = 0;
      eResize.width = 0;
      eKey.state = 0;
      eKey.key = 0;
      eMouse.posX = 0;
      eMouse.posY = 0;
      eMouse.state = 0;
   }
   eWinInfo( eInit *_ptr ) {
      type = 0;
      eInitPointer = _ptr;
      eResize.posX = 0;
      eResize.posY = 0;
      eResize.height = 0;
      eResize.width = 0;
      eKey.state = 0;
      eKey.key = 0;
      eMouse.posX = 0;
      eMouse.posY = 0;
      eMouse.state = 0;
   }
};


}

#endif // E_WINDOW_DATA_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
