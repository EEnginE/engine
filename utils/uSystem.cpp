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

#include "uSystem.hpp"
#include "uLog.hpp"
#include "uConfig.hpp"
#include "defines.hpp"

#if UNIX
#include <unistd.h>
#include <pwd.h>
#endif // UNIX

#if WINDOWS
#include <windows.h>
#include <lmcons.h>
#include <stdlib.h>

#undef POINT
#include <shlobj.h>

#endif


#define BOOST_FILESYSTEM_NO_DEPRECATED
#include FILESYSTEM_INCLUDE
#include <regex>
#include <iostream>

namespace e_engine {

uSystem SYSTEM;

uSystem::uSystem() {
#if UNIX
   uid_t tempUserID = geteuid();
   struct passwd *pw = nullptr;

   pw = getpwuid( tempUserID );

   vUserLogin = pw->pw_name;
   vUserName = pw->pw_gecos;
   vUserHome = pw->pw_dir;

#elif WINDOWS
   DWORD lUsername_DWORD = UNLEN + 1;
   TCHAR lUsername_TCSTR[UNLEN + 1];
   if ( GetUserName( lUsername_TCSTR, &lUsername_DWORD ) == 0 ) {
      wLOG( "Failed to get Username (WINDOWS)" );
      lUsername_TCSTR[0] = 0;
   }

   TCHAR lAppData_LPTSTR[MAX_PATH + 1];
   if ( SHGetFolderPath( NULL, CSIDL_APPDATA, NULL, 0, lAppData_LPTSTR ) != S_OK ) {
      wLOG( "Failed to get AppData (WINDOWS)" );
   }

#if defined UNICODE || defined _UNICODE
   char lUsername_CSTR[UNLEN + 1];
   char lAppData_CSTR[MAX_PATH + 1];

   if ( wcstombs( lUsername_CSTR, lUsername_TCSTR, UNLEN ) == (size_t)-1 ) {
      wLOG( "Failed to convert a WCHAR string to a CHAR string using wcstombs (USERNAME)" );
   }

   if ( wcstombs( lAppData_CSTR, lAppData_LPTSTR, MAX_PATH ) == (size_t)-1 ) {
      wLOG( "Failed to convert a WCHAR string to a CHAR string using wcstombs (APPDATA)" );
   }

   vUserName = lUsername_CSTR;
   vUserLogin = vUserName;
   vUserHome = lAppData_CSTR;
#else
   vUserName = lUsername_TCSTR;
   vUserLogin = vUserName;
   vUserHome = lAppData_LPTSTR;
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
   if ( vMainConfigDir.empty() ) {

      // Replace all bad characters with '-'
      std::regex ex( "[^A-Za-z0-9.\\*]" );
      const char *fmt = "-";
      std::string out = std::regex_replace( GlobConf.config.appName, ex, fmt );

#if UNIX
      std::string dir1_str = vUserHome + "/.";
      dir1_str += out;
      std::string dir2_str = vUserHome + "/.config/";
      dir2_str += out;

      FILESYSTEM_NAMESPACE::path dir1( dir1_str );
      FILESYSTEM_NAMESPACE::path dir2( dir2_str );
      FILESYSTEM_NAMESPACE::path configDir( vUserHome + "/.config" );
      bool dir1_exists, dir2_exists;
      bool dir1_noDir, dir2_noDir = false;
      bool dotConfigExists;

      try {
         // Is there a $HOME/.NAME dir already?
         if ( FILESYSTEM_NAMESPACE::exists( dir1 ) ) {
            if ( FILESYSTEM_NAMESPACE::is_directory( dir1 ) ) {
               dir1_exists = true;
               dir1_noDir = false;
            } else {
               dir1_exists = false;
               dir1_noDir = true;
            }
         } else {
            dir1_exists = false;
            dir1_noDir = false;
         }

         // If the $HOME/.cnfig dir doesn't exist use the $HOME location
         if ( FILESYSTEM_NAMESPACE::exists( configDir ) ) {
            if ( FILESYSTEM_NAMESPACE::is_directory( configDir ) ) {
               dotConfigExists = true;
               // Is there a $HOME/.config/NAME dir already?
               if ( FILESYSTEM_NAMESPACE::exists( dir2 ) ) {
                  if ( FILESYSTEM_NAMESPACE::is_directory( dir2 ) ) {
                     dir2_exists = true;
                     dir1_noDir = false;
                  } else {
                     dir2_exists = false;
                     dir2_noDir = true;
                  }
               } else {
                  dir2_exists = false;
                  dir2_noDir = false;
               }
            } else {
               GlobConf.config.unixPathType = false;
               dir2_exists = false;
               dir2_noDir = true;
               dotConfigExists = false;
            }
         } else {
            GlobConf.config.unixPathType = false;
            dir2_exists = false;
            dir2_noDir = true;
            dotConfigExists = false;
         }

         // Only $HOME/.NAME already exists
         if ( dir1_exists && !dir2_exists ) {
            vMainConfigDir = dir1_str;
            return vMainConfigDir;
         }

         // Only $HOME/.config/NAME already exists
         if ( !dir1_exists && dir2_exists ) {
            vMainConfigDir = dir2_str;
            return vMainConfigDir;
         }

         // Both already exist
         if ( dir1_exists && dir2_exists ) {
            // Choose the preferred
            if ( GlobConf.config.unixPathType ) {
               vMainConfigDir = dir2_str;
               return vMainConfigDir;
            } else {
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
            }
         }

         // None already exists

         // Choose the preferred path
         if ( GlobConf.config.unixPathType ) {
            // There is no directory file $HOME/.config/NAME
            if ( !dir2_noDir ) {
               FILESYSTEM_NAMESPACE::create_directory( dir2 );
               vMainConfigDir = dir2_str;
               return vMainConfigDir;
            } else if ( !dir1_noDir ) {
               FILESYSTEM_NAMESPACE::create_directory( dir1 );
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
               // Remove the file $HOME/.config/NAME and create the dir
            } else if ( dotConfigExists ) {
               FILESYSTEM_NAMESPACE::remove( dir2 );
               FILESYSTEM_NAMESPACE::create_directory( dir2 );
               vMainConfigDir = dir2_str;
               return vMainConfigDir;
            } else {
               FILESYSTEM_NAMESPACE::remove( dir1 );
               FILESYSTEM_NAMESPACE::create_directory( dir1 );
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
            }
         } else {
            // There is no directory file $HOME/.NAME
            if ( !dir1_noDir ) {
               FILESYSTEM_NAMESPACE::create_directory( dir1 );
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
            } else if ( !dir2_noDir && dotConfigExists ) {
               FILESYSTEM_NAMESPACE::create_directory( dir2 );
               vMainConfigDir = dir2_str;
               return vMainConfigDir;
               // Remove the file $HOME/.NAME and create the dir
            } else {
               FILESYSTEM_NAMESPACE::remove( dir1 );
               FILESYSTEM_NAMESPACE::create_directory( dir1 );
               vMainConfigDir = dir1_str;
               return vMainConfigDir;
            }
         }
      } catch ( const FILESYSTEM_NAMESPACE::filesystem_error &ex ) { eLOG( ex.what() ); }

#elif WINDOWS
      vMainConfigDir = vUserHome + '\\' + out;

      FILESYSTEM_NAMESPACE::path dir1( vMainConfigDir );

      try {
         if ( FILESYSTEM_NAMESPACE::exists( dir1 ) ) {
            if ( !FILESYSTEM_NAMESPACE::is_directory( dir1 ) ) {
               FILESYSTEM_NAMESPACE::remove( dir1 );
               FILESYSTEM_NAMESPACE::create_directory( dir1 );
            }
         } else { FILESYSTEM_NAMESPACE::create_directory( dir1 ); }

         if ( !FILESYSTEM_NAMESPACE::exists( dir1 ) ) {
            wLOG( "Failed to create / select the main config dir ", vMainConfigDir );
            vMainConfigDir.clear();
            return "";
         }

         if ( !FILESYSTEM_NAMESPACE::is_directory( dir1 ) ) {
            wLOG( "Failed to create / select the main config dir ", vMainConfigDir );
            vMainConfigDir.clear();
            return "";
         }

      } catch ( const FILESYSTEM_NAMESPACE::filesystem_error &ex ) { eLOG( ex.what() ); }
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
   if ( vLogFilePath.empty() ) {
      if ( GlobConf.config.logSubFolder.empty() ) {
         vLogFilePath = getMainConfigDirPath();
         return vLogFilePath;
      } else {

#if UNIX
         std::string temp = getMainConfigDirPath() + "/";
#elif WINDOWS
         std::string temp = getMainConfigDirPath() + "\\";
#endif

         temp += GlobConf.config.logSubFolder;

         FILESYSTEM_NAMESPACE::path logPath( temp );

         try {
            if ( FILESYSTEM_NAMESPACE::exists( logPath ) ) {
               if ( FILESYSTEM_NAMESPACE::is_directory( logPath ) ) {
                  vLogFilePath = temp;
                  return vLogFilePath;
               } else {
                  FILESYSTEM_NAMESPACE::remove( logPath );
                  FILESYSTEM_NAMESPACE::create_directory( logPath );
                  vLogFilePath = temp;
                  return vLogFilePath;
               }
            } else {
               FILESYSTEM_NAMESPACE::create_directory( logPath );
               vLogFilePath = temp;
               return vLogFilePath;
            }
         } catch ( const FILESYSTEM_NAMESPACE::filesystem_error &ex ) {
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
   if ( vConfigFilePath.empty() ) {
      if ( GlobConf.config.logSubFolder.empty() ) {
         vConfigFilePath = getMainConfigDirPath();
         return vConfigFilePath;
      } else {

#if UNIX
         std::string temp = getMainConfigDirPath() + "/";
#elif WINDOWS
         std::string temp = getMainConfigDirPath() + "\\";
#endif
         temp += GlobConf.config.logSubFolder;

         FILESYSTEM_NAMESPACE::path confPath( temp );

         try {
            if ( FILESYSTEM_NAMESPACE::exists( confPath ) ) {
               if ( FILESYSTEM_NAMESPACE::is_directory( confPath ) ) {
                  vConfigFilePath = temp;
                  return vConfigFilePath;
               } else {
                  FILESYSTEM_NAMESPACE::remove( confPath );
                  FILESYSTEM_NAMESPACE::create_directory( confPath );
                  vConfigFilePath = temp;
                  return vConfigFilePath;
               }
            } else {
               FILESYSTEM_NAMESPACE::create_directory( confPath );
               vConfigFilePath = temp;
               return vConfigFilePath;
            }
         } catch ( const FILESYSTEM_NAMESPACE::filesystem_error &ex ) {
            eLOG( ex.what() ); // LOG wont work
         }
      }
   }
   return vConfigFilePath;
}
}





// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
