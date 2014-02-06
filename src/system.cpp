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
#include <stdlib.h>

#undef POINT
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

   vUserLogin = pw->pw_name;
   vUserName  = pw->pw_gecos;
   vUserHome  = pw->pw_dir;

#elif WINDOWS
   DWORD lUsername_DWORD = UNLEN + 1;
   TCHAR lUsername_TCSTR[UNLEN + 1];
   if ( GetUserName ( lUsername_TCSTR, &lUsername_DWORD ) == 0 ) {
      wLOG "Failed to get Username (WINDOWS)" END
      lUsername_TCSTR[0] = 0;
   }

   TCHAR lAppData_LPTSTR[MAX_PATH + 1];
   if ( SHGetFolderPath ( NULL, CSIDL_APPDATA, NULL, 0, lAppData_LPTSTR ) != S_OK ) {
      wLOG "Failed to get AppData (WINDOWS)" END
   }

#if defined UNICODE || defined _UNICODE
   char lUsername_CSTR[UNLEN + 1];
   char lAppData_CSTR[MAX_PATH + 1];

   if ( wcstombs ( lUsername_CSTR, lUsername_TCSTR, UNLEN ) == ( size_t ) - 1 ) {
      wLOG "Failed to convert a WCHAR string to a CHAR string using wcstombs (USERNAME)" END
   }

   if ( wcstombs ( lAppData_CSTR, lAppData_LPTSTR, MAX_PATH ) == ( size_t ) - 1 ) {
      wLOG "Failed to convert a WCHAR string to a CHAR string using wcstombs (APPDATA)" END
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
 * Serch for an existing main config dir and if it doesn't
 * exists  creates it. The settings from \c WinData.config
 * will be used.
 *
 * \returns The main config dir path
 * \sa _eWindowData
 */
std::string eSystem::getMainConfigDirPath() {
   if ( vMainConfigDir.empty() ) {

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
            vMainConfigDir = dir1_str;
            return vMainConfigDir;
         }

         // Only $HOME/.config/NAME already exists
         if ( ! dir1_exists && dir2_exists ) {
            vMainConfigDir = dir2_str;
            return vMainConfigDir;
         }

         // Both already exists
         if ( dir1_exists && dir2_exists ) {
            // Chose the prefered
            if ( WinData.config.unixPathType ) {
               vMainConfigDir = dir2_str;
               return vMainConfigDir;
            } else {
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
            }
         }

         // No one already exists

         // Chose the prefered path
         if ( WinData.config.unixPathType ) {
            // There is no none directory file $HOME/.config/NAME
            if ( ! dir2_noDir ) {
               boost::filesystem::create_directory ( dir2 );
               vMainConfigDir = dir2_str;
               return vMainConfigDir;
            } else if ( ! dir1_noDir ) {
               boost::filesystem::create_directory ( dir1 );
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
               // Remove the file $HOME/.config/NAME and create the dir
            } else if ( dotConfigExists ) {
               boost::filesystem::remove ( dir2 );
               boost::filesystem::create_directory ( dir2 );
               vMainConfigDir = dir2_str;
               return vMainConfigDir;
            } else {
               boost::filesystem::remove ( dir1 );
               boost::filesystem::create_directory ( dir1 );
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
            }
         } else {
            // There is no none directory file $HOME/.NAME
            if ( ! dir1_noDir ) {
               boost::filesystem::create_directory ( dir1 );
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
            } else if ( ! dir2_noDir && dotConfigExists ) {
               boost::filesystem::create_directory ( dir2 );
               vMainConfigDir = dir2_str;
               return vMainConfigDir;
               // Remove the file $HOME/.NAME and create the dir
            } else {
               boost::filesystem::remove ( dir1 );
               boost::filesystem::create_directory ( dir1 );
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
            }
         }
      } catch ( const boost::filesystem::filesystem_error &ex ) {
         eLOG ex.what() END
      }

#elif WINDOWS
      vMainConfigDir = vUserHome + '\\' + out;

      boost::filesystem::path dir1 ( vMainConfigDir );

      try {
         if ( boost::filesystem::exists ( dir1 ) ) {
            if ( ! boost::filesystem::is_directory ( dir1 ) ) {
               boost::filesystem::remove ( dir1 );
               boost::filesystem::create_directory ( dir1 );
            }
         } else {
            boost::filesystem::create_directory ( dir1 );
         }

         if ( ! boost::filesystem::exists ( dir1 ) ) {
            wLOG "Failed to craete / select the main config dir " ADD vMainConfigDir END
            vMainConfigDir.clear();
            return "";
         }
         
         if ( ! boost::filesystem::is_directory ( dir1 ) ) {
            wLOG "Failed to craete / select the main config dir " ADD vMainConfigDir END
            vMainConfigDir.clear();
            return "";
         }

      } catch ( const boost::filesystem::filesystem_error &ex ) {
         eLOG ex.what() END
      }
#endif

   }
   return vMainConfigDir;
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
   if ( vLogFilePath.empty() ) {
      if ( WinData.config.logSubFolder.empty() ) {
         vLogFilePath = getMainConfigDirPath();
         return vLogFilePath;
      } else {

#if UNIX
         std::string temp = getMainConfigDirPath() + "/";
#elif WINDOWS
         std::string temp = getMainConfigDirPath() + "\\";
#endif

         temp += WinData.config.logSubFolder;

         boost::filesystem::path logPath ( temp );

         try {
            if ( boost::filesystem::exists ( logPath ) ) {
               if ( boost::filesystem::is_directory ( logPath ) ) {
                  vLogFilePath = temp;
                  return vLogFilePath;
               } else {
                  boost::filesystem::remove ( logPath );
                  boost::filesystem::create_directory ( logPath );
                  vLogFilePath = temp;
                  return vLogFilePath;
               }
            } else {
               boost::filesystem::create_directory ( logPath );
               vLogFilePath = temp;
               return vLogFilePath;
            }
         } catch ( const boost::filesystem::filesystem_error &ex ) {
            std::cerr << ex.what() << std::endl; // LOG wont work
         }

      }
   }
   return vLogFilePath;
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
   if ( vConfigFilePath.empty() ) {
      if ( WinData.config.logSubFolder.empty() ) {
         vConfigFilePath = getMainConfigDirPath();
         return vConfigFilePath;
      } else {

#if UNIX
         std::string temp = getMainConfigDirPath() + "/";
#elif WINDOWS
         std::string temp = getMainConfigDirPath() + "\\";
#endif
         temp += WinData.config.logSubFolder;

         boost::filesystem::path confPath ( temp );

         try {
            if ( boost::filesystem::exists ( confPath ) ) {
               if ( boost::filesystem::is_directory ( confPath ) ) {
                  vConfigFilePath = temp;
                  return vConfigFilePath;
               } else {
                  boost::filesystem::remove ( confPath );
                  boost::filesystem::create_directory ( confPath );
                  vConfigFilePath = temp;
                  return vConfigFilePath;
               }
            } else {
               boost::filesystem::create_directory ( confPath );
               vConfigFilePath = temp;
               return vConfigFilePath;
            }
         } catch ( const boost::filesystem::filesystem_error &ex ) {
            eLOG ex.what() END // LOG wont work
         }

      }
   }
   return vConfigFilePath;
}


}


















// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
