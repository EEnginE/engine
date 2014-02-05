/// \file system.cpp
/// \brief \b Classes: \a eSystem
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

#include "system.hpp"
#include "log.hpp"
#include "window_data.hpp"
#include "defines.hpp"

#if UNIX
#include <unistd.h>
#include <pwd.h>

#elif WINDOWS
#include <windows.h>
#include <lmcons.h>

#undef POINT // conflicts with shlobj.h
#include <shlobj.h>

#endif


#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

namespace e_engine {

eSystem SYSTEM;

eSystem::eSystem() {
#if UNIX
   uid_t tempUserID = geteuid();
   struct passwd *pw = 0;

   pw = getpwuid ( tempUserID );

   userLogin = pw->pw_name;
   userName  = pw->pw_gecos;
   userHome  = pw->pw_dir;

#elif WINDOWS
   DWORD lUsername_DWORD = UNLEN + 1;
   TCHAR lUsername_TCSTR[UNLEN + 1];
   if ( GetUserName ( lUsername_TCSTR, &lUsername_DWORD ) == 0 ) {
      wLOG "Failed to get Username" END
      lUsername_TCSTR[0] = 0;
   }

   WCHAR **lAppData_PWSTR = 0;
   if ( SHGetKnownFolderPath ( FOLDERID_ProgramData, 0, NULL, lAppData_PWSTR ) != S_OK ) {

   }

   CoTaskMemFree ( lAppData_PWSTR );


#ifdef _UNICODE

#endif

#endif

   mainConfigDir.clear();
   logFilePath.clear();
   configFilePath.clear();
}

/*!
 * \brief Get the main config dir path
 *
 * Serch for an existing main config dir and if it doesn't
 * exists  creates it. The settings from \c WinData.config
 * will be used.
 *
 * \returns The main config dir path
 * \sa _eWindowData
 */
std::string eSystem::getMainConfigDirPath() {
   if ( mainConfigDir.empty() ) {

      // Replace all bad characters with '-'
      boost::regex ex ( "[^A-Za-z0-9.\\*]" );
      const char *fmt =  "-";
      std::string out = boost::regex_replace ( WinData.config.appName, ex, fmt );

#if UNIX
      std::string dir1_str = userHome + "/.";
      dir1_str += out;
      std::string dir2_str = userHome + "/.config/";
      dir2_str += out;

      boost::filesystem::path dir1 ( dir1_str );
      boost::filesystem::path dir2 ( dir2_str );
      boost::filesystem::path configDir ( userHome + "/.config" );
      bool dir1_exists, dir2_exists;
      bool dir1_noDir,  dir2_noDir;
      bool dotConfigExists;

      try {
         // Is there a $HOME/.NAME dir already?
         if ( boost::filesystem::exists ( dir1 ) ) {
            if ( boost::filesystem::is_directory ( dir1 ) ) {
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

         // If the $HOME/.cnfig dir doesn't exits use the $HOME location
         if ( boost::filesystem::exists ( configDir ) ) {
            if ( boost::filesystem::is_directory ( configDir ) ) {
               dotConfigExists = true;
               // Is there a $HOME/.config/NAME dir already?
               if ( boost::filesystem::exists ( dir2 ) ) {
                  if ( boost::filesystem::is_directory ( dir2 ) ) {
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
               WinData.config.unixPathType = false;
               dir2_exists = false;
               dir2_noDir  = true;
               dotConfigExists = false;
            }
         } else {
            WinData.config.unixPathType = false;
            dir2_exists = false;
            dir2_noDir  = true;
            dotConfigExists = false;
         }

         // Only $HOME/.NAME aleady exits
         if ( dir1_exists && ! dir2_exists ) {
            mainConfigDir = dir1_str;
            return mainConfigDir;
         }

         // Only $HOME/.config/NAME already exists
         if ( ! dir1_exists && dir2_exists ) {
            mainConfigDir = dir2_str;
            return mainConfigDir;
         }

         // Both already exists
         if ( dir1_exists && dir2_exists ) {
            // Chose the prefered
            if ( WinData.config.unixPathType ) {
               mainConfigDir = dir2_str;
               return mainConfigDir;
            } else {
               mainConfigDir = dir1_str;
               return mainConfigDir;
            }
         }

         // No one already exists

         // Chose the prefered path
         if ( WinData.config.unixPathType ) {
            // There is no none directory file $HOME/.config/NAME
            if ( ! dir2_noDir ) {
               boost::filesystem::create_directory ( dir2 );
               mainConfigDir = dir2_str;
               return mainConfigDir;
            } else if ( ! dir1_noDir ) {
               boost::filesystem::create_directory ( dir1 );
               mainConfigDir = dir1_str;
               return mainConfigDir;
               // Remove the file $HOME/.config/NAME and create the dir
            } else if ( dotConfigExists ) {
               boost::filesystem::remove ( dir2 );
               boost::filesystem::create_directory ( dir2 );
               mainConfigDir = dir2_str;
               return mainConfigDir;
            } else {
               boost::filesystem::remove ( dir1 );
               boost::filesystem::create_directory ( dir1 );
               mainConfigDir = dir1_str;
               return mainConfigDir;
            }
         } else {
            // There is no none directory file $HOME/.NAME
            if ( ! dir1_noDir ) {
               boost::filesystem::create_directory ( dir1 );
               mainConfigDir = dir1_str;
               return mainConfigDir;
            } else if ( ! dir2_noDir && dotConfigExists ) {
               boost::filesystem::create_directory ( dir2 );
               mainConfigDir = dir2_str;
               return mainConfigDir;
               // Remove the file $HOME/.NAME and create the dir
            } else {
               boost::filesystem::remove ( dir1 );
               boost::filesystem::create_directory ( dir1 );
               mainConfigDir = dir1_str;
               return mainConfigDir;
            }
         }
      } catch ( const boost::filesystem::filesystem_error &ex ) {
         std::cerr << ex.what() << std::endl; // LOG wont work
      }

#elif WINDOWS
      std::string temp1 = userHome + "/";
      temp1 += out;
#endif

   }
   return mainConfigDir;
}


/*!
 * \brief Get the log file dir
 *
 * Serch for an existing log file dir in the main config dir and
 * if it doesn't exists creates it. The settings from \c WinData.config
 * will be used.
 *
 * \returns The log file dir path
 * \sa _eWindowData
 */
std::string eSystem::getLogFilePath() {
   if ( logFilePath.empty() ) {
      if ( WinData.config.logSubFolder.empty() ) {
         logFilePath = getMainConfigDirPath();
         return logFilePath;
      } else {
         std::string temp = getMainConfigDirPath() + "/";
         temp += WinData.config.logSubFolder;

         boost::filesystem::path logPath ( temp );

         try {
            if ( boost::filesystem::exists ( logPath ) ) {
               if ( boost::filesystem::is_directory ( logPath ) ) {
                  logFilePath = temp;
                  return logFilePath;
               } else {
                  boost::filesystem::remove ( logPath );
                  boost::filesystem::create_directory ( logPath );
                  logFilePath = temp;
                  return logFilePath;
               }
            } else {
               boost::filesystem::create_directory ( logPath );
               logFilePath = temp;
               return logFilePath;
            }
         } catch ( const boost::filesystem::filesystem_error &ex ) {
            std::cerr << ex.what() << std::endl; // LOG wont work
         }

      }
   }
   return logFilePath;
}


/*!
 * \brief Get the log file dir
 *
 * Serch for an existing log file dir in the main config dir and
 * if it doesn't exists creates it. The settings from \c WinData.config
 * will be used.
 *
 * \returns The log file dir path
 * \sa _eWindowData
 */
std::string eSystem::getConfigFilePath() {
   if ( configFilePath.empty() ) {
      if ( WinData.config.logSubFolder.empty() ) {
         configFilePath = getMainConfigDirPath();
         return configFilePath;
      } else {
         std::string temp = getMainConfigDirPath() + "/";
         temp += WinData.config.logSubFolder;

         boost::filesystem::path confPath ( temp );

         try {
            if ( boost::filesystem::exists ( confPath ) ) {
               if ( boost::filesystem::is_directory ( confPath ) ) {
                  configFilePath = temp;
                  return configFilePath;
               } else {
                  boost::filesystem::remove ( confPath );
                  boost::filesystem::create_directory ( confPath );
                  configFilePath = temp;
                  return configFilePath;
               }
            } else {
               boost::filesystem::create_directory ( confPath );
               configFilePath = temp;
               return configFilePath;
            }
         } catch ( const boost::filesystem::filesystem_error &ex ) {
            eLOG ex.what() END // LOG wont work
         }

      }
   }
   return configFilePath;
}


}


















// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
