/*!
 * \file uFileIO.hpp
 * \brief \b Classes: \a uFileIO
 */

#ifndef U_FILE_IO_HPP
#define U_FILE_IO_HPP

#include <string>
#include <vector>
#include "engine_utils_Export.hpp"

namespace e_engine {

class utils_EXPORT uFileIO {
   public:
      typedef std::string::const_iterator C_ITERATOR;
      typedef std::string::iterator       ITERATOR;
      
      typedef std::string                 TYPE;

   private:
      std::string  vFilePath_str;
      TYPE         vData;
      bool         vFileRead_B;

   public:
      uFileIO() :                                            vFileRead_B( false ) {}
      uFileIO( std::string _file ) : vFilePath_str( _file ), vFileRead_B( false ) {}
      void        setFilePath( std::string _file );
      std::string getFilePath();

      C_ITERATOR  begin() const {return vData.begin();}
      ITERATOR    begin()       {return vData.begin();}

      C_ITERATOR  end()   const {return vData.end();}
      ITERATOR    end()         {return vData.end();}

      bool        isFileRead() {return vFileRead_B;}

      int         read( bool _autoReload = true );
      int         write( TYPE const& _data, bool _overWrite = false );
      void        clear();

      int         operator()( bool _autoReload = true ) {return read( _autoReload );}

      virtual ~uFileIO() {}
};


}

#endif // U_FILE_IO_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
