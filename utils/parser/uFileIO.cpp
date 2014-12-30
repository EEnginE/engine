/*!
 * \file uFileIO.cpp
 * \brief \b Classes: \a uFileIO
 */

#include "uFileIO.hpp"
#include "uLog.hpp"
#include <boost/filesystem.hpp>

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

   boost::filesystem::path lFilePath_BFS( vFilePath_str.c_str() );

   if ( !boost::filesystem::exists( lFilePath_BFS ) ) {
      eLOG( "File ", vFilePath_str, " does not exists" );
      return 3;
   }

   if ( !boost::filesystem::is_regular_file( lFilePath_BFS ) ) {
      eLOG( vFilePath_str, " is not a file!" );
      return 4;
   }

   FILE *lFile = fopen( vFilePath_str.c_str(), "r" );
   if ( lFile == nullptr ) {
      eLOG( "Unable to open ", vFilePath_str );
      return 5;
   }

   int c;

   while ( ( c = fgetc( lFile ) ) != EOF )
      vData += (char)c;

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
   boost::filesystem::path lFilePath_BFS( vFilePath_str.c_str() );

   if ( boost::filesystem::exists( lFilePath_BFS ) ) {
      if ( !_overWrite ) {
         eLOG( "File ", vFilePath_str, " already exists -- do not overwrite" );
         return 2;
      }

      if ( !boost::filesystem::is_regular_file( lFilePath_BFS ) ) {
         eLOG( vFilePath_str, " is not a file! -- can not overwrite" );
         return 3;
      }

      wLOG( "File ", vFilePath_str, " already exists -- overwrite" );

      if ( !boost::filesystem::remove( lFilePath_BFS ) ) {
         eLOG( "Failed to remove '", vFilePath_str, "'" );
         return 4;
      }
   }

   FILE *lFile = fopen( vFilePath_str.c_str(), "w" );
   if ( lFile == nullptr ) {
      eLOG( "Unable to open ", vFilePath_str );
      return 5;
   }

   for ( char ch : _data )
      fputc( ch, lFile );

   fclose( lFile );

   return 1;
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
