/*!
 * \file rLoader_3D_OBJ.cpp
 * \brief \b Classes: \a rLoader_3D_OBJ
 */

#include "rLoader_3D_f_OBJ.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include "uLog.hpp"
#include "uFileIO.hpp"

#define DO_NOT_FAIL_PARSING 1

namespace e_engine {

rLoader_3D_f_OBJ::rLoader_3D_f_OBJ() {
   vIsDataLoaded_B = false;
}

rLoader_3D_f_OBJ::rLoader_3D_f_OBJ( std::string _file ) {
   vIsDataLoaded_B = false;
   vFilePath_str   = _file;
}


namespace spirit  = boost::spirit;
namespace qi      = boost::spirit::qi;
namespace ascii   = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template<class Iterator>
struct objGrammar_float : qi::grammar<Iterator, internal::_3D_Data_RAWF()> {
   static void unsupported_f( std::string _what ) {
      wLOG( "OBJ Parser: '", _what, "' is not supported" );
   }

   static void objName_f( std::string _name ) {
      dLOG( "Found object ", _name );
   }

   static void mtllib_f( std::string _lib ) {
      wLOG( "The 'mtllib' command in obj files is currently not supported (do not load '", _lib, "')" );
   }

   static void usemtl_f( std::string _mtl ) {
      wLOG( "The 'usemtl' command in obj files is currently not supported (do not set the material to '", _mtl, "')" );
   }

   static void smooth_f( std::string _smooth ) {
      wLOG( "The 's' command in obj files is currently not supported (do not set the smoothing group to '", _smooth, "')" );
   }

   objGrammar_float() : objGrammar_float::base_type( start ) {
      using ascii::char_;
      using qi::float_;
      using qi::uint_;
      using qi::int_;
      using phoenix::push_back;
      using phoenix::at_c;

      string    = +( char_ - '\n' );
      comment   = qi::lit( "#" ) >> *string >> '\n';
      space     = ( char_( ' ' ) | char_( '\t' ) );

      objName   = qi::lit( "o" ) >> +space >> string[&objGrammar_float::objName_f] >> '\n';
      groupName = qi::lit( "g" ) >> +space >> string[&objGrammar_float::objName_f] >> '\n';

      mtllib    = qi::lit( "mtllib" ) >> +space >> string[&objGrammar_float::mtllib_f] >> '\n';
      usemtl    = qi::lit( "usemtl" ) >> +space >> string[&objGrammar_float::usemtl_f] >> '\n';
      smooth    = qi::lit( "s" )      >> +space >> string[&objGrammar_float::smooth_f] >> '\n';

#if DO_NOT_FAIL_PARSING
      other     = *ascii::space >> string[&objGrammar_float::unsupported_f] >> '\n';
#endif

      start     =
         +( *ascii::space >>
            ( qi::lit( "v" ) >> +space
              >> float_[push_back( at_c<0>( spirit::_val ), spirit::_1 )] >> +space
              >> float_[push_back( at_c<0>( spirit::_val ), spirit::_1 )] >> +space
              >> float_[push_back( at_c<0>( spirit::_val ), spirit::_1 )] >> *space >> '\n' ) |

              ( qi::lit( "vt" ) >> +space
              >> float_[push_back( at_c<1>( spirit::_val ), spirit::_1 )] >> +space
              >> float_[push_back( at_c<1>( spirit::_val ), spirit::_1 )] >> *space >> '\n' ) |

              ( qi::lit( "vn" ) >> +space
              >> float_[push_back( at_c<2>( spirit::_val ), spirit::_1 )] >> +space
              >> float_[push_back( at_c<2>( spirit::_val ), spirit::_1 )] >> +space
              >> float_[push_back( at_c<2>( spirit::_val ), spirit::_1 )] >> *space >> '\n' ) |

            ( qi::lit( "f" ) >> +space
              >> uint_[push_back( at_c<3>( spirit::_val ), spirit::_1 )]
              >> -( qi::lit( "/" ) >> -uint_[push_back( at_c<4>( spirit::_val ), spirit::_1 )] >> -( qi::lit( "/" ) >> uint_[push_back( at_c<5>( spirit::_val ), spirit::_1 )] ) )
              >> +space

              >> uint_[push_back( at_c<3>( spirit::_val ), spirit::_1 )]
              >> -( qi::lit( "/" ) >> -uint_[push_back( at_c<4>( spirit::_val ), spirit::_1 )] >> -( qi::lit( "/" ) >> uint_[push_back( at_c<5>( spirit::_val ), spirit::_1 )] ) )
              >> +space

              >> uint_[push_back( at_c<3>( spirit::_val ), spirit::_1 )]
              >> -( qi::lit( "/" ) >> -uint_[push_back( at_c<4>( spirit::_val ), spirit::_1 )] >> -( qi::lit( "/" ) >> uint_[push_back( at_c<5>( spirit::_val ), spirit::_1 )] ) )
              >> *space >> '\n' )  |

            objName   |
            groupName |
            mtllib    |
            comment   |
            usemtl    |
            smooth
#if DO_NOT_FAIL_PARSING
            | other
#endif
          );

   }

   qi::rule<Iterator, std::string()> string;
   qi::rule<Iterator, std::string()> comment;
   qi::rule<Iterator, std::string()> space;

   qi::rule<Iterator, std::string()> objName;
   qi::rule<Iterator, std::string()> groupName;

   qi::rule<Iterator, std::string()> mtllib;
   qi::rule<Iterator, std::string()> usemtl;
   qi::rule<Iterator, std::string()> smooth;

#if DO_NOT_FAIL_PARSING
   qi::rule<Iterator, std::string()> other;
#endif

   qi::rule<Iterator, internal::_3D_Data_RAWF()> start;
};


/*!
 * \brief loads the 3D content frome the OBJ file
 * \returns 1 on success
 * \returns 2 if there was a parsing error
 * \returns 3 if the OBJ file doesn't exists
 * \returns 4 if the OBJ file is not a regular file
 * \returns 5 if the OBJ file is not readable
 * \returns 6 if already loaded
 */
int rLoader_3D_f_OBJ::load() {
   if ( vIsDataLoaded_B )
      return 6;

   uFileIO lFile( vFilePath_str );
   int lRet = lFile();
   if ( lRet != 1 ) return lRet;

   objGrammar_float<uFileIO::C_ITERATOR> lGrammar;

   uFileIO::C_ITERATOR lStartIter = lFile.begin();
   uFileIO::C_ITERATOR lEndIter   = lFile.end();
   bool lReturn = qi::parse( lStartIter, lEndIter, lGrammar, vDataRaw );

   if ( ( ! lReturn ) || ( lStartIter != lEndIter ) ) {
      eLOG( "Failed to parse '", vFilePath_str, "'" );
      return 2;
   }

   vIsDataLoaded_B = true;

   return 1;
}








}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;

