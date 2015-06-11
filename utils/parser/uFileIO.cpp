/*!
 * \file uFileIO.cpp
 * \brief \b Classes: \a uFileIO
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

#include "uFileIO.hpp"
#include "uLog.hpp"
#include FILESYSTEM_INCLUDE

namespace e_engine {

void uFileIO::clear() {
   vFileRead_B = false;
   vData.clear();
   vData.resize( 0 );
}

std::string uFileIO::getFilePath() { return vFilePath_str; }

void uFileIO::setFilePath( std::string _file ) { vFilePath_str = _file; }

/*!
 * \brief Reads the file
 *
 * \param[in] _autoReload when true, runns clear(); when file is already read (default: false)
 *
 * \returns 1 if everything went fine
 * \returns 2 if the file was already read (and _autoReload == false)
 * \returns 3 if the file doesn't exists
 * \returns 4 if the file is not a regular file
 * \returns 5 if the file is not readable
 */
int uFileIO::read( bool _autoReload ) {
   if ( vFileRead_B == true && _autoReload == false )
      return 2;

   FILESYSTEM_NAMESPACE::path lFilePath_BFS( vFilePath_str.c_str() );

   if ( !FILESYSTEM_NAMESPACE::exists( lFilePath_BFS ) ) {
      eLOG( "File '", vFilePath_str, "' does not exists" );
      return 3;
   }

   if ( !FILESYSTEM_NAMESPACE::is_regular_file( lFilePath_BFS ) ) {
      eLOG( "'", vFilePath_str, "' is not a file!" );
      return 4;
   }

   FILE *lFile = fopen( vFilePath_str.c_str(), "rb" );
   if ( lFile == nullptr ) {
      eLOG( "Unable to open ", vFilePath_str );
      return 5;
   }

   int c;
   auto lSize = FILESYSTEM_NAMESPACE::file_size( lFilePath_BFS );

   if ( lSize != static_cast<uintmax_t>( -1 ) ) {
      vData.resize( lSize );

      for ( auto &i : vData ) {
         if ( ( c = fgetc( lFile ) ) == EOF ) {
            wLOG( "File size missmatch (to small)! File: '", vFilePath_str, "'" );
            break;
         }

         i = static_cast<char>( c );
      }

      if ( ( c = fgetc( lFile ) ) != EOF )
         wLOG( "File size missmatch (to large)! File: '", vFilePath_str, "'" );

   } else {
      wLOG( "Unable to obtain the file size!" );

      while ( ( c = fgetc( lFile ) ) != EOF )
         vData += static_cast<char>( c );
   }

   fclose( lFile );

   vFileRead_B = true;

   return 1;
}

/*!
 * \brief Writes the file
 *
 * \param[in] _data      what to write
 * \param[in] _overWrite when true, replaces the content of a file
 *
 * \returns 1 if everything went fine
 * \returns 2 if the file already exisits read (and _overWrite == false)
 * \returns 3 if the file exists and is not a regular file
 * \returns 4 if the file exists and is not removable
 * \returns 5 if the file is not writable
 */
int uFileIO::write( const uFileIO::TYPE &_data, bool _overWrite ) {
   FILESYSTEM_NAMESPACE::path lFilePath_BFS( vFilePath_str.c_str() );

   if ( FILESYSTEM_NAMESPACE::exists( lFilePath_BFS ) ) {
      if ( !_overWrite ) {
         eLOG( "File '", vFilePath_str, "' already exists -- do not overwrite" );
         return 2;
      }

      if ( !FILESYSTEM_NAMESPACE::is_regular_file( lFilePath_BFS ) ) {
         eLOG( "'", vFilePath_str, "' is not a file! -- can not overwrite" );
         return 3;
      }

      wLOG( "File '", vFilePath_str, "' already exists -- overwrite" );

      if ( !FILESYSTEM_NAMESPACE::remove( lFilePath_BFS ) ) {
         eLOG( "Failed to remove '", vFilePath_str, "'" );
         return 4;
      }
   }

   FILE *lFile = fopen( vFilePath_str.c_str(), "wb" );
   if ( lFile == nullptr ) {
      eLOG( "Unable to open '", vFilePath_str, "'" );
      return 5;
   }

   for ( char ch : _data )
      fputc( ch, lFile );

   fclose( lFile );

   return 1;
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
