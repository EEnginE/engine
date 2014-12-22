/*!
 * \file uParserJSON.hpp
 * \brief \b Classes: \a uParserJSON
 */

#ifndef U_PARSER_JSON_HPP
#define U_PARSER_JSON_HPP

#include "uParserJSON_data.hpp"

namespace e_engine {

class uParserJSON {
   private:
      std::string vFilePath_str;
      bool        vIsParsed;

      std::string vWriteIndent_str;

      uJSON_data vData;

      std::string::const_iterator vIter;
      std::string::const_iterator vEnd;

      unsigned int vCurrentLine = 1;

      void writeValue( uJSON_data const &_data, std::string &_worker, std::string _level, bool _array );
      void prepareString( std::string const &_in, std::string &_out );

      bool continueWhitespace();

      bool parseObject( e_engine::uJSON_data &lCurrentObject );
      bool parseArray( e_engine::uJSON_data &_currentObject );
      bool parseValue( e_engine::uJSON_data &_currentObject, const std::string &_name );

   public:
      virtual ~uParserJSON() {}
      uParserJSON() :                                            vIsParsed( false ), vWriteIndent_str( "  " ) {}
      uParserJSON( std::string _file ) : vFilePath_str( _file ), vIsParsed( false ), vWriteIndent_str( "  " ) {}

      void setFile( std::string _file );
      int  parse();
      void clear();

      int  write( uJSON_data const &_data, bool _overwriteIfNeeded = false );

      uJSON_data  getData()   {return vData;}
      uJSON_data  *getDataP() {return &vData;}

      void        setWriteIndent( std::string _in );

      bool        getIsParsed() const;
      std::string getFilePath() const;
};

}

#endif // U_PARSER_JSON_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
