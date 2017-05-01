/*!
 * \file uConfig.hpp
 * \brief \b Classes: \a uConfig
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

#pragma once

#include "defines.hpp"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace e_engine {

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


  struct __uConfig_Vulkan {
    __uConfig_Vulkan();

    VkSampleCountFlagBits samples;
    VkFormat              preferedSurfaceFormat;
    bool                  enableVSync;

    /*!
     * \brief Reset to default
     */
    void reset();
  } vk;

  //   _   _               _
  //  | | | |             (_)
  //  | | | | ___ _ __ ___ _  ___  _ __
  //  | | | |/ _ \ '__/ __| |/ _ \| '_ \
   //  \ \_/ /  __/ |  \__ \ | (_) | | | |
  //   \___/ \___|_|  |___/_|\___/|_| |_|
  //

  struct __uConfig_Versions {
    //! \todo Add vulkan stuff here
    __uConfig_Versions();

    int vulkanMajorVersion;
    int vulkanMinorVersion;
    int vulkanPatchVersion;

    /*!
     * \brief Reset to default
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
     */
    void reset();
  } config;

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

  // Versions

  _uConfig();
};

/*!
 * \brief The main object from _uConfig
 * \sa _uConfig __uConfig_FBA
 */
extern _uConfig GlobConf;
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
