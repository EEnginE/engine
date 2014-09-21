/*!
 * \file uParserJSON.cpp
 * \brief \b Classes: \a uParserJSON
 */

#include "uParserJSON.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/lexical_cast.hpp>

#include "uLog.hpp"
#include "uFileIO.hpp"


namespace {
typedef std::vector<::e_engine::uJSON_data>  VALUES;
}

namespace e_engine {

/*!
 * \brief Gets wether or not the file is loaded and parsed
 * \returns The state of the file being loaded and parsed
 */

bool uParserJSON::getIsParsed() const {
   return vIsParsed;
}

/*!
 * \brief Gets the path of the file to parse
 * \returns The path of the file to parse
 */
std::string uParserJSON::getFilePath() const {
   return vFilePath_str;
}


/*!
 * \brief Sets the file to parse
 * \param[in] _file The file to parse
 *
 * \note This will NOT parse the file! You have to manually parse it with parse()
 *
 * \returns Nothing
 */
void uParserJSON::setFile( std::string _file ) {
   vFilePath_str = _file;
}

/*!
 * \brief Clears the memory
 * \returns Nothing
 */
void uParserJSON::clear() {
   vIsParsed = false;
   vData.value_str.clear();
   vData.id.clear();
   vData.value_array.clear();
   vData.value_obj.clear();
   vData.type = __JSON_NOT_SET__;
}


namespace spirit  = boost::spirit;
namespace qi      = boost::spirit::qi;
namespace ascii   = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template<class ITER>
struct json_grammar : qi::grammar<ITER, uJSON_data(), ascii::space_type> {

   json_grammar() : json_grammar::base_type( start ) {
      using qi::lexeme;
      using qi::lit;
      using qi::double_;
      using ascii::char_;

      using qi::_val;

      using phoenix::at_c;
      using phoenix::push_back;
      using phoenix::ref;

      string     = lexeme[ '"' >> +( ( char_ - '"' - '\\' ) | ( '\\' >> char_ ) )[_val += qi::_1 ] >> '"' ];
      number     = lexeme[ double_[ _val = qi::_1 ] ];
      boolean    = lexeme[( lit( "true" )[_val = true] | lit( "false" )[_val = false] ) ];
      undefined  = lexeme[ "null" ];

      valueArray =
            ( ( string    [ at_c<1>( _val ) = qi::_1 ] ) [ at_c<6>( _val ) = JSON_STRING ] ) |
            ( ( number    [ at_c<2>( _val ) = qi::_1 ] ) [ at_c<6>( _val ) = JSON_NUMBER ] ) |
            ( ( boolean   [ at_c<3>( _val ) = qi::_1 ] ) [ at_c<6>( _val ) = JSON_BOOL   ] ) |
            ( ( undefined ) [ at_c<6>( _val ) = JSON_NULL   ] ) |
            (
                  '['
                  >>           valueArray [ push_back( at_c<4>( _val ), qi::_1 ) ]
                  >> *( ',' >> valueArray [ push_back( at_c<4>( _val ), qi::_1 ) ] )
                  >> lit( ']' )    [ at_c<6>( _val ) = JSON_ARRAY ]
            ) |
            (
                  '{'
                  >>           value      [ push_back( at_c<5>( _val ), qi::_1 ) ]
                  >> *( ',' >> value      [ push_back( at_c<5>( _val ), qi::_1 ) ] )
                  >> lit( '}' )    [ at_c<6>( _val ) = JSON_OBJECT ]
            );

      value      =
            string[at_c<0>( _val ) = qi::_1] >> ':' >>
            (
                  ( ( string    [ at_c<1>( _val ) = qi::_1 ] ) [ at_c<6>( _val ) = JSON_STRING ] ) |
                  ( ( number    [ at_c<2>( _val ) = qi::_1 ] ) [ at_c<6>( _val ) = JSON_NUMBER ] ) |
                  ( ( boolean   [ at_c<3>( _val ) = qi::_1 ] ) [ at_c<6>( _val ) = JSON_BOOL   ] ) |
                  ( ( undefined ) [ at_c<6>( _val ) = JSON_NULL   ] ) |
                  (
                        '['
                        >>           valueArray [ push_back( at_c<4>( _val ), qi::_1 ) ]
                        >> *( ',' >> valueArray [ push_back( at_c<4>( _val ), qi::_1 ) ] )
                        >> lit( ']' )    [ at_c<6>( _val ) = JSON_ARRAY ]
                  ) |
                  (
                        '{'
                        >>           value      [ push_back( at_c<5>( _val ), qi::_1 ) ]
                        >> *( ',' >> value      [ push_back( at_c<5>( _val ), qi::_1 ) ] )
                        >> lit( '}' )    [ at_c<6>( _val ) = JSON_OBJECT ]
                  )
            );

      start    =
            '{'
            >>           value[push_back( at_c<5>( _val ), qi::_1 )]
            >> *( ',' >> value[push_back( at_c<5>( _val ), qi::_1 )] )
            >> lit( '}' )[at_c<6>( _val ) = JSON_OBJECT] ;
   }

   qi::rule<ITER, std::string(), ascii::space_type> string;
   qi::rule<ITER, double(),      ascii::space_type> number;
   qi::rule<ITER, bool(),        ascii::space_type> boolean;
   qi::rule<ITER, void(),        ascii::space_type> undefined;

   qi::rule<ITER, uJSON_data(),  ascii::space_type> value;
   qi::rule<ITER, uJSON_data(),  ascii::space_type> valueArray;
   qi::rule<ITER, uJSON_data(),  ascii::space_type> start;
};

/*!
 * \brief loads the content of the JSON file
 * \returns 1 on success
 * \returns 2 if there was a parsing error
 * \returns 3 if the JSON file doesn't exists
 * \returns 4 if the JSON file is not a regular file
 * \returns 5 if the JSON file is not readable
 * \returns 6 if already parsed
 */
int uParserJSON::parse() {
   if( vIsParsed )
      return 6;

   uFileIO lFile( vFilePath_str );
   int lRet = lFile();
   if( lRet != 1 ) return lRet;

   using boost::spirit::ascii::space;

   json_grammar<uFileIO::C_ITERATOR> lGrammar;

   uFileIO::C_ITERATOR lStartIter = lFile.begin();
   uFileIO::C_ITERATOR lEndIter   = lFile.end();
   bool lReturn = qi::phrase_parse( lStartIter, lEndIter, lGrammar, space, vData );

   if( ( ! lReturn ) || ( lStartIter != lEndIter ) ) {
      eLOG( "Failed to parse '", vFilePath_str, "'", " At '", *lStartIter, "'", " (", lStartIter - lFile.begin(), ")" );
      return 2;
   }
   iLOG( "Successfully parsed '", vFilePath_str, "' (JSON)" );

   vIsParsed = true;

   return 1;
}

void uParserJSON::setWriteIndent( std::string _in ) {
   vWriteIndent_str = _in;
}


/*!
 * \brief Generates a JSON file from a uJSON_data structure
 *
 * \param[in] _data              the data to write
 * \param[in] _overwriteIfNeeded When true overwrites the file if it exists
 *
 * \returns 1  if everything went fine
 * \returns 2  if the file already exisits read (and _overWrite == false)
 * \returns 3  if the file exists and is not a regular file
 * \returns 4  if the file exists and is not removable
 * \returns 5  if the file is not writable
 * \returns -2 if _data has not the type JSON_OBJECT
 */
int uParserJSON::write( const uJSON_data &_data, bool _overwriteIfNeeded ) {
   if( _data.type != JSON_OBJECT )
      return -2;

   std::string lOutput = "{\n";
   for( unsigned int i = 0; i < _data.value_obj.size(); ++i ) {
      writeValue( _data.value_obj[i], lOutput, vWriteIndent_str, false );
      if( i != ( _data.value_obj.size() - 1 ) )
         lOutput += ",";

      lOutput += "\n";
   }
   lOutput += "}\n";

   uFileIO lFile( vFilePath_str );
   return lFile.write( lOutput, _overwriteIfNeeded );;
}

void uParserJSON::prepareString( const std::string &_in, std::string &_out ) {
   for( char ch : _in ) {
      if( ch == '"' || ch == '\\' )
         _out.append( 1, '\\' );

      _out.append( 1, ch );
   }
}


void uParserJSON::writeValue( const uJSON_data &_data, std::string &_worker, std::string _level, bool _array ) {
   unsigned int i;
   std::string  lTemp_str;

   if( !_array )
      _worker += _level + "\"" + _data.id + "\"" + ": ";
   else
      _worker += _level;

   switch( _data.type ) {
      case JSON_STRING:
         prepareString( _data.value_str, lTemp_str );
         _worker += "\"" + lTemp_str + "\"";
         break;
      case JSON_NUMBER:
         _worker += boost::lexical_cast<std::string>( _data.value_num );
         break;
      case JSON_BOOL:
         _worker += ( _data.value_bool ? "true" : "false" );
         break;
      case JSON_NULL:
         _worker += "null";
         break;
      case JSON_ARRAY:
         _worker += "[\n";
         for( i = 0; i < _data.value_array.size(); ++i ) {
            writeValue( _data.value_array[i], _worker, _level  + vWriteIndent_str, true );
            if( i != ( _data.value_array.size() - 1 ) )
               _worker += ",";

            _worker += "\n";
         }
         _worker += _level + "]";
         break;
      case JSON_OBJECT:
         _worker += "{\n";
         for( i = 0; i < _data.value_obj.size(); ++i ) {
            writeValue( _data.value_obj[i], _worker, _level  + vWriteIndent_str, false );
            if( i != ( _data.value_obj.size() - 1 ) )
               _worker += ",";

            _worker += "\n";
         }
         _worker += _level + "}";
         break;
      default:
         wLOG( "Incomplete JSON data structure. Skipp unknown parts.  ( File: '", vFilePath_str, "' )" );
         break;
   }
}




}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 


