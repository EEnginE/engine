/*!
 * \file uSystem.cpp
 * \brief \b Classes: \a uSystem
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

#include "defines.hpp"
#include "uSystem.hpp"
#include "uConfig.hpp"
#include "uLog.hpp"

#if UNIX
#include <pwd.h>
#include <unistd.h>
#endif // UNIX

#if WINDOWS
#include <lmcons.h>
#include <stdlib.h>
#include <windows.h>

#undef POINT
#include <shlobj.h>

#endif


#if __cplusplus <= 201402L || true //! \todo FIX THIS when C++17 is released
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#include <iostream>
#include <regex>

namespace e_engine {

uSystem SYSTEM;

uSystem::uSystem() {
#if UNIX
  uid_t          tempUserID = geteuid();
  struct passwd *pw         = nullptr;

  pw = getpwuid(tempUserID);

  vUserLogin = pw->pw_name;
  vUserName  = pw->pw_gecos;
  vUserHome  = pw->pw_dir;

#elif WINDOWS
  DWORD lUsername_DWORD = UNLEN + 1;
  TCHAR lUsername_TCSTR[UNLEN + 1];
  if (GetUserName(lUsername_TCSTR, &lUsername_DWORD) == 0) {
    wLOG("Failed to get Username (WINDOWS)");
    lUsername_TCSTR[0] = 0;
  }

  TCHAR lAppData_LPTSTR[MAX_PATH + 1];
  if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, lAppData_LPTSTR) != S_OK) {
    wLOG("Failed to get AppData (WINDOWS)");
  }

#if defined UNICODE || defined _UNICODE
  char lUsername_CSTR[UNLEN + 1];
  char lAppData_CSTR[MAX_PATH + 1];

  if (wcstombs(lUsername_CSTR, lUsername_TCSTR, UNLEN) == (size_t)-1) {
    wLOG("Failed to convert a WCHAR string to a CHAR string using wcstombs (USERNAME)");
  }

  if (wcstombs(lAppData_CSTR, lAppData_LPTSTR, MAX_PATH) == (size_t)-1) {
    wLOG("Failed to convert a WCHAR string to a CHAR string using wcstombs (APPDATA)");
  }

  vUserName  = lUsername_CSTR;
  vUserLogin = vUserName;
  vUserHome  = lAppData_CSTR;
#else
  vUserName  = lUsername_TCSTR;
  vUserLogin = vUserName;
  vUserHome  = lAppData_LPTSTR;
#endif

#endif

  vMainConfigDir.clear();
  vLogFilePath.clear();
  vConfigFilePath.clear();
}

/*!
 * \brief Get the main config dir path
 *
 * Search for an existing main config dir and if it doesn't
 * exist,  creates it.
 * The settings from \c GlobConf.config will be used.
 *
 * \returns The main config dir path
 * \sa _uConfig
 */
std::string uSystem::getMainConfigDirPath() {
  if (vMainConfigDir.empty()) {

    // Replace all bad characters with '-'
    std::regex  ex("[^A-Za-z0-9.\\*]");
    const char *fmt = "-";
    std::string out = std::regex_replace(GlobConf.config.appName, ex, fmt);

#if UNIX
    std::string dir1_str = vUserHome + "/.";
    dir1_str += out;
    std::string dir2_str = vUserHome + "/.config/";
    dir2_str += out;

    fs::path dir1(dir1_str);
    fs::path dir2(dir2_str);
    fs::path configDir(vUserHome + "/.config");
    bool     dir1_exists, dir2_exists;
    bool     dir1_noDir, dir2_noDir = false;
    bool     dotConfigExists;

    try {
      // Is there a $HOME/.NAME dir already?
      if (fs::exists(dir1)) {
        if (fs::is_directory(dir1)) {
          dir1_exists = true;
          dir1_noDir  = false;
        } else {
          dir1_exists = false;
          dir1_noDir  = true;
        }
      } else {
        dir1_exists = false;
        dir1_noDir  = false;
      }

      // If the $HOME/.cnfig dir doesn't exist use the $HOME location
      if (fs::exists(configDir)) {
        if (fs::is_directory(configDir)) {
          dotConfigExists = true;
          // Is there a $HOME/.config/NAME dir already?
          if (fs::exists(dir2)) {
            if (fs::is_directory(dir2)) {
              dir2_exists = true;
              dir1_noDir  = false;
            } else {
              dir2_exists = false;
              dir2_noDir  = true;
            }
          } else {
            dir2_exists = false;
            dir2_noDir  = false;
          }
        } else {
          GlobConf.config.unixPathType = false;
          dir2_exists                  = false;
          dir2_noDir                   = true;
          dotConfigExists              = false;
        }
      } else {
        GlobConf.config.unixPathType = false;
        dir2_exists                  = false;
        dir2_noDir                   = true;
        dotConfigExists              = false;
      }

      // Only $HOME/.NAME already exists
      if (dir1_exists && !dir2_exists) {
        vMainConfigDir = dir1_str;
        return vMainConfigDir;
      }

      // Only $HOME/.config/NAME already exists
      if (!dir1_exists && dir2_exists) {
        vMainConfigDir = dir2_str;
        return vMainConfigDir;
      }

      // Both already exist
      if (dir1_exists && dir2_exists) {
        // Choose the preferred
        if (GlobConf.config.unixPathType) {
          vMainConfigDir = dir2_str;
          return vMainConfigDir;
        } else {
          vMainConfigDir = dir1_str;
          return vMainConfigDir;
        }
      }

      // None already exists

      // Choose the preferred path
      if (GlobConf.config.unixPathType) {
        // There is no directory file $HOME/.config/NAME
        if (!dir2_noDir) {
          fs::create_directory(dir2);
          vMainConfigDir = dir2_str;
          return vMainConfigDir;
        } else if (!dir1_noDir) {
          fs::create_directory(dir1);
          vMainConfigDir = dir1_str;
          return vMainConfigDir;
          // Remove the file $HOME/.config/NAME and create the dir
        } else if (dotConfigExists) {
          fs::remove(dir2);
          fs::create_directory(dir2);
          vMainConfigDir = dir2_str;
          return vMainConfigDir;
        } else {
          fs::remove(dir1);
          fs::create_directory(dir1);
          vMainConfigDir = dir1_str;
          return vMainConfigDir;
        }
      } else {
        // There is no directory file $HOME/.NAME
        if (!dir1_noDir) {
          fs::create_directory(dir1);
          vMainConfigDir = dir1_str;
          return vMainConfigDir;
        } else if (!dir2_noDir && dotConfigExists) {
          fs::create_directory(dir2);
          vMainConfigDir = dir2_str;
          return vMainConfigDir;
          // Remove the file $HOME/.NAME and create the dir
        } else {
          fs::remove(dir1);
          fs::create_directory(dir1);
          vMainConfigDir = dir1_str;
          return vMainConfigDir;
        }
      }
    } catch (const fs::filesystem_error &fsError) { eLOG(fsError.what()); }

#elif WINDOWS
    vMainConfigDir = vUserHome + '\\' + out;

    fs::path dir1(vMainConfigDir);

    try {
      if (fs::exists(dir1)) {
        if (!fs::is_directory(dir1)) {
          fs::remove(dir1);
          fs::create_directory(dir1);
        }
      } else {
        fs::create_directory(dir1);
      }

      if (!fs::exists(dir1)) {
        wLOG("Failed to create / select the main config dir ", vMainConfigDir);
        vMainConfigDir.clear();
        return "";
      }

      if (!fs::is_directory(dir1)) {
        wLOG("Failed to create / select the main config dir ", vMainConfigDir);
        vMainConfigDir.clear();
        return "";
      }

    } catch (const fs::filesystem_error &ex) { eLOG(ex.what()); }
#endif
  }
  return vMainConfigDir;
}


/*!
 * \brief Get the log file dir
 *
 * Search for an existing log file dir in the main config dir and
 * if it doesn't exist, creates it.
 * The settings from \c GlobConf.config will be used.
 *
 * \returns The log file dir path
 * \sa _uConfig
 */
std::string uSystem::getLogFilePath() {
  if (vLogFilePath.empty()) {
    if (GlobConf.config.logSubFolder.empty()) {
      vLogFilePath = getMainConfigDirPath();
      return vLogFilePath;
    } else {

#if UNIX
      std::string temp = getMainConfigDirPath() + "/";
#elif WINDOWS
      std::string temp = getMainConfigDirPath() + "\\";
#endif

      temp += GlobConf.config.logSubFolder;

      fs::path logPath(temp);

      try {
        if (fs::exists(logPath)) {
          if (fs::is_directory(logPath)) {
            vLogFilePath = temp;
            return vLogFilePath;
          } else {
            fs::remove(logPath);
            fs::create_directory(logPath);
            vLogFilePath = temp;
            return vLogFilePath;
          }
        } else {
          fs::create_directory(logPath);
          vLogFilePath = temp;
          return vLogFilePath;
        }
      } catch (const fs::filesystem_error &ex) {
        std::cerr << ex.what() << std::endl; // LOG wont work
      }
    }
  }
  return vLogFilePath;
}


/*!
 * \brief Get the log file dir
 *
 * Search for an existing log file dir in the main config dir and
 * if it doesn't exist, creates it.
 * The settings from \c GlobConf.config will be used.
 *
 * \returns The log file dir path
 * \sa _uConfig
 */
std::string uSystem::getConfigFilePath() {
  if (vConfigFilePath.empty()) {
    if (GlobConf.config.logSubFolder.empty()) {
      vConfigFilePath = getMainConfigDirPath();
      return vConfigFilePath;
    } else {

#if UNIX
      std::string temp = getMainConfigDirPath() + "/";
#elif WINDOWS
      std::string temp = getMainConfigDirPath() + "\\";
#endif
      temp += GlobConf.config.logSubFolder;

      fs::path confPath(temp);

      try {
        if (fs::exists(confPath)) {
          if (fs::is_directory(confPath)) {
            vConfigFilePath = temp;
            return vConfigFilePath;
          } else {
            fs::remove(confPath);
            fs::create_directory(confPath);
            vConfigFilePath = temp;
            return vConfigFilePath;
          }
        } else {
          fs::create_directory(confPath);
          vConfigFilePath = temp;
          return vConfigFilePath;
        }
      } catch (const fs::filesystem_error &ex) {
        eLOG(ex.what()); // LOG wont work
      }
    }
  }
  return vConfigFilePath;
}
} // namespace e_engine




// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
