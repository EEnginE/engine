/*!
 * \file uParserJSON.hpp
 * \brief \b Classes: \a uParserJSON
 */

#ifndef U_PARSER_JSON_HPP
#define U_PARSER_JSON_HPP

#include "uParserJSON_data.hpp"
#include "engine_utils_Export.hpp"

namespace e_engine {

class utils_EXPORT uParserJSON {
   private:
      std::string vFilePath_str;
      bool        vIsParsed;
      
      std::string vWriteIndent_str;
      
      uJSON_data vData;
      
      void writeValue( uJSON_data const& _data, std::string &_worker, std::string _level, bool _array );
      void prepareString( std::string const& _in, std::string &_out );

   public:
      virtual ~uParserJSON() {}
      uParserJSON() :                                            vIsParsed( false ), vWriteIndent_str( "  " ) {}
      uParserJSON( std::string _file ) : vFilePath_str( _file ), vIsParsed( false ), vWriteIndent_str( "  " ) {}
      
      void setFile( std::string _file );
      int  parse();
      void clear();
      
      int  write( uJSON_data const& _data, bool _overwriteIfNeeded = false );
      
      uJSON_data  getData()   {return vData;}
      uJSON_data  *getDataP() {return &vData;}
      
      void        setWriteIndent( std::string _in );
      
      bool        getIsParsed() const;
      std::string getFilePath() const;
};

}

#endif // U_PARSER_JSON_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
